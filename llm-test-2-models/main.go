package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"path/filepath"
	"strings"
)

const (
	apiKey = "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"
	model  = "deepseek/deepseek-r1-0528-qwen3-8b:free"
	srcDir = "../controllers"
	outDir = "./generated_tests"
)

func main() {
	os.MkdirAll(outDir, 0755)

	systemMsg := `You are a C++ unit‑test generator. Output ONLY valid GoogleTest .cc code for the test file. Do NOT include any analysis, commentary, or <think> tags.`

	userTemplate := `Generate GoogleTest unit tests for the following C++ code.
Cover all public classes/functions, no duplicates, include headers, use TEST/TEST_F.
Source header:
<<INCLUDE_HEADER>>
Source file:
<<INCLUDE_SOURCE>>`

	files, _ := filepath.Glob(filepath.Join(srcDir, "*.cc"))

	for _, ccFile := range files {
		base := filepath.Base(ccFile)
		name := strings.TrimSuffix(base, ".cc")
		headerPath := filepath.Join(srcDir, name+".h")

		ccContent, _ := os.ReadFile(ccFile)
		headerContent, _ := os.ReadFile(headerPath)

		userMsg := strings.ReplaceAll(userTemplate, "<<INCLUDE_HEADER>>", string(headerContent))
		userMsg = strings.ReplaceAll(userMsg, "<<INCLUDE_SOURCE>>", string(ccContent))

		body := map[string]interface{}{
			"model": model,
			"messages": []map[string]string{
				{"role": "system", "content": systemMsg},
				{"role": "user", "content": userMsg},
			},
		}

		jsonPayload, _ := json.Marshal(body)

		req, _ := http.NewRequest("POST", "https://openrouter.ai/api/v1/chat/completions", bytes.NewBuffer(jsonPayload))
		req.Header.Set("Content-Type", "application/json")
		req.Header.Set("HTTP-Referer", "https://github.com/ShardenduMishra22") // or your actual site
		req.Header.Set("Authorization", "Bearer "+apiKey)

		resp, err := http.DefaultClient.Do(req)
		if err != nil {
			fmt.Printf("Request failed for %s: %v\n", base, err)
			continue
		}
		defer resp.Body.Close()

		raw, _ := ioutil.ReadAll(resp.Body)
		fmt.Printf("Response for %s:\n%s\n", base, string(raw))

		var result struct {
			Choices []struct {
				Message struct {
					Content string `json:"content"`
				} `json:"message"`
			} `json:"choices"`
		}

		json.Unmarshal(raw, &result)
		if len(result.Choices) == 0 {
			fmt.Printf("Empty response for %s\n", base)
			continue
		}

		content := result.Choices[0].Message.Content

		// Strip leading ``` and language tag if present
		if strings.HasPrefix(content, "```") {
			if idx := strings.Index(content, "\n"); idx != -1 {
				content = content[idx+1:]
			}
		}
		// Strip trailing ```
		if strings.HasSuffix(content, "```") {
			content = strings.TrimSuffix(content, "```")
		}
		content = strings.TrimSpace(content)

		outFile := filepath.Join(outDir, name+"_test.cc")
		ioutil.WriteFile(outFile, []byte(content), 0644)

		fmt.Printf("Generated test for %s\n", base)
	}
}
