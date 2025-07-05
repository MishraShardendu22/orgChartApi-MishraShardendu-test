package main

import (
	"bytes"
	"encoding/json"
	"io"
	"log"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
	"sync"

	"github.com/joho/godotenv"
)

var (
	apiKey = ""
	model  = "deepseek/deepseek-r1-0528-qwen3-8b:free"
	srcDir = "../controllers"
	outDir = "../test"
)

func Generate() {
	if os.Getenv("RUN_ENV") != "production" {
		_ = godotenv.Load(".env")
	}

	apiKey = os.Getenv("API")
	runtime.GOMAXPROCS(runtime.NumCPU())
	_ = os.MkdirAll(outDir, 0755)

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
		ccFile := ccFile
		wg.Add(1)

		go func() {
			defer wg.Done()

			base := filepath.Base(ccFile)
			name := strings.TrimSuffix(base, ".cc")
			headerPath := filepath.Join(srcDir, name+".h")

			ccContent, err1 := os.ReadFile(ccFile)
			headerContent, err2 := os.ReadFile(headerPath)
			if err1 != nil || err2 != nil {
				log.Printf("Read error on %s: %v %v", base, err1, err2)
				return
			}

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
			req.Header.Set("Authorization", "Bearer "+apiKey)

			resp, err := http.DefaultClient.Do(req)
			if err != nil {
				log.Printf("Request failed for %s: %v", base, err)
				return
			}
			defer resp.Body.Close()

			raw, _ := io.ReadAll(resp.Body)

			var result struct {
				Choices []struct {
					Message struct {
						Content string `json:"content"`
					} `json:"message"`
				} `json:"choices"`
			}

			json.Unmarshal(raw, &result)
			if len(result.Choices) == 0 {
				log.Printf("Empty response for %s", base)
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
			_ = os.WriteFile(outFile, []byte(content), 0644)
		}()
	}

	wg.Wait()
}

func refineTestsWithBuildLog(buildLog string) {
	files, _ := filepath.Glob(filepath.Join(outDir, "*_test.cc"))
	systemPrompt := `You are a test fixer. Fix ONLY what causes build errors. No comments, no markdown.`
	userTemplate := `C++ test file:
<<CODE>>

Build log:
<<LOG>>

Fix the test file.`

	var wg sync.WaitGroup
	for _, testFile := range files {
		testFile := testFile
		wg.Add(1)

		go func() {
			defer wg.Done()
			content, err := os.ReadFile(testFile)
			if err != nil {
				log.Printf("Read error: %s", testFile)
				return
			}

			userMsg := strings.ReplaceAll(userTemplate, "<<CODE>>", string(content))
			userMsg = strings.ReplaceAll(userMsg, "<<LOG>>", buildLog)

			body := map[string]interface{}{
				"model": model,
				"messages": []map[string]string{
					{"role": "system", "content": systemPrompt},
					{"role": "user", "content": userMsg},
				},
			}

			jsonPayload, _ := json.Marshal(body)
			req, _ := http.NewRequest("POST", "https://openrouter.ai/api/v1/chat/completions", bytes.NewBuffer(jsonPayload))
			req.Header.Set("Content-Type", "application/json")
			req.Header.Set("Authorization", "Bearer "+apiKey)

			resp, err := http.DefaultClient.Do(req)
			if err != nil {
				log.Printf("Fixing failed for %s: %v", testFile, err)
				return
			}
			defer resp.Body.Close()

			raw, _ := io.ReadAll(resp.Body)

			var result struct {
				Choices []struct {
					Message struct {
						Content string `json:"content"`
					} `json:"message"`
				} `json:"choices"`
			}
			json.Unmarshal(raw, &result)
			if len(result.Choices) == 0 {
				log.Printf("No response for %s", testFile)
				return
			}

			content = []byte(strings.TrimSpace(strings.TrimPrefix(result.Choices[0].Message.Content, "```")))
			_ = os.WriteFile(testFile, content, 0644)
			log.Printf("Fixed test: %s", testFile)
		}()
	}
	wg.Wait()
}

func runBuildAndCaptureLog() string {
	cmd := exec.Command("bash", "-c", "mkdir -p ../build && cd ../build && cmake .. && make -j$(nproc)")
	out, err := cmd.CombinedOutput()
	if err != nil {
		log.Printf("Build failed: %v", err)
	}
	return string(out)
}

func main() {
	Generate()
	buildLog := runBuildAndCaptureLog()

	if strings.Contains(buildLog, "error:") {
		log.Println("Build failed. Starting refinement.")
		refineTestsWithBuildLog(buildLog)
	} else {
		log.Println("Build passed. Proceeding to coverage.")
	}
}
