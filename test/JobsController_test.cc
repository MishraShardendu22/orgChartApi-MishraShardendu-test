cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drogon/HttpServer.h>
#include <drogon/orm/Query.h>
#include <drogon/drogon.h>
#include <drogon/Model.h>
#include <vector>
#include <memory>
#include <string>
#include "JobsController.h"

using namespace ::testing;
using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::org_chart;

class JobsControllerTestFixture : public Test {
protected:
    JobsController controller;

    HttpResponsePtr createMockResponse(int statusCode, const std::string& content = "{}") {
        auto response = std::make_shared<HttpResponse>();
        response->setStatusCode(statusCode);
        auto json = Json::Value(Json::ValueType::objectValue);
        json["content"] = content;
        response->setBody(JsonUtils::writeJson(json));
        return response;
    }

    std::vector<Job> createMockJobs(int count) {
        std::vector<Job> jobs;
        for (int i = 0; i < count; i++) {
            Job job;
            job.setId(i + 1);
            job.setTitle("Title " + std::to_string(i));
            job.setValueOfDescription("Description " + std::to_string(i));
            jobs.push_back(job);
        }
        return jobs;
    }

    void initializeMockDb(const std::vector<Job>& mockJobs) {
        auto dbClient = drogon::app().getDbClient();
        if (!dbClient) {
            return;
        }

        Mapper<Job> mapper(dbClient);
        mapper.clear();
        for (const auto& job : mockJobs) {
            mapper.insert(job);
        }
    }
};

TEST_F(JobsControllerTestFixture, Get_JobsReturnsOkWithJobs) {
    auto offset = 0;
    auto limit = 10;
    auto sortField = "id";
    auto sortOrder = "asc";

    auto mockJobs = createMockJobs(2);
    initializeMockDb(mockJobs);

    auto callback = [](const HttpResponsePtr& response) {
        EXPECT_EQ(response->getStatusCode(), HttpStatusCode::k200OK);
        Json::Value json = Json::parse(response->getBody());
        const auto& data = json["data"];
        ASSERT_EQ(data.size(), 2);
    };

    controller.get(nullptr, callback);
}

TEST_F(JobsControllerTestFixture, Get_JobNotFound) {
    int jobId = 999;

    auto callback = [](const HttpResponsePtr& response) {
        EXPECT_EQ(response->getStatusCode(), HttpStatusCode::k404NotFound);
    };

    controller.getOne(nullptr, callback, jobId);
}

TEST_F(JobsControllerTestFixture, Create_JobCreatedSuccessfully) {
    Job createdJob;
    createdJob.setId(0); // Let model generate ID
    createdJob.setTitle("New Job");
    createdJob.setValueOfDescription("New Description");

    auto callback = [](const HttpResponsePtr& response) {
        EXPECT_EQ(response->getStatusCode(), HttpStatusCode::k201Created);
    };

    controller.createOne(nullptr, [job = createdJob](const HttpResponsePtr&) {}, createdJob);
}

TEST_F(JobsControllerTestFixture, Update_JobUpdated) {
    // This is a complex test - typically require a real database setup
    // Simplified example only
    int jobId = 1;
    Job updatedJob;
    updatedJob.setId(jobId);
    updatedJob.setTitle("Updated Title");

    auto callback = [](const HttpResponsePtr& response) {
        EXPECT_EQ(response->getStatusCode(), HttpStatusCode::k204NoContent);
    };

    controller.updateOne(nullptr, callback, jobId, updatedJob);
}

TEST_F(JobsControllerTestFixture, Delete_JobDeleted) {
    int jobId = 1;

    auto callback = [](const HttpResponsePtr& response) {
        EXPECT_EQ(response->getStatusCode(), HttpStatusCode::k204NoContent);
    };

    controller.deleteOne(nullptr, callback, jobId);
}

TEST_F(JobsControllerTestFixture, GetJobPersons_JobHasPersons) {
    int jobId = 1;
    // Setup mock persons
    Job job;
    job.setId(jobId);
    // Normally would fill with real logic

    auto callback = [](const HttpResponsePtr& response) {
        Json::Value json = Json::parse(response->getBody());
        const auto& data = json["data"];
        ASSERT_EQ(data.size(), 2);
        EXPECT_EQ(response->getStatusCode(), HttpStatusCode::k200OK);
    };

    controller.getJobPersons(nullptr, [job](const HttpResponsePtr&) {}, jobId);
}
```