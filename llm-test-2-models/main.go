package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"path/filepath"
	"runtime"
	"strings"
	"sync"
)

const (
	apiKey = "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"
	model  = "deepseek/deepseek-r1-0528-qwen3-8b:free"
	srcDir = "../controllers"
	outDir = "../test"
)

func main() {
	runtime.GOMAXPROCS(runtime.NumCPU())
	os.MkdirAll(outDir, 0755)

	systemMsg := `You are a C++ unit test generator. Output only valid raw C++ GoogleTest code. No markdown, no text, no comments, no tags. Only C++ code.`

	userTemplate := `Generate GoogleTest unit tests for the following C++ code.
Cover all public classes/functions, no duplicates, include headers, use TEST/TEST_F.
Source header:
<<INCLUDE_HEADER>>
Source file:
<<INCLUDE_SOURCE>>`

	files, _ := filepath.Glob(filepath.Join(srcDir, "*.cc"))

	var wg sync.WaitGroup
	for _, ccFile := range files {
		ccFile := ccFile // capture loop variable
		wg.Add(1)

		go func() {
			defer wg.Done()

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
			req.Header.Set("HTTP-Referer", "https://github.com/ShardenduMishra22")
			req.Header.Set("Authorization", "Bearer "+apiKey)

			resp, err := http.DefaultClient.Do(req)
			if err != nil {
				fmt.Printf("Request failed for %s: %v\n", base, err)
				return
			}
			defer resp.Body.Close()

			raw, _ := ioutil.ReadAll(resp.Body)
			// fmt.Printf("Response for %s:\n%s\n", base, string(raw))

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
				return
			}

			content := result.Choices[0].Message.Content

			if strings.HasPrefix(content, "```") {
				if idx := strings.Index(content, "\n"); idx != -1 {
					content = content[idx+1:]
				}
			}
			content = strings.TrimSuffix(content, "```")
			content = strings.TrimSpace(content)

			outFile := filepath.Join(outDir, name+"_test.cc")
			ioutil.WriteFile(outFile, []byte(content), 0644)

			// fmt.Printf("Generated test for %s\n", base)
		}()
	}

	wg.Wait()
}
