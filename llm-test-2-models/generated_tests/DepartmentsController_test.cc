```cpp
#include <gtest/gtest.h>
#include <drogon/HttpAppFramework.h>
#include <memory>
#include <vector>
#include <optional>
#include "DepartmentsController.h"
#include "../models/Department.h"
#include "../models/Person.h"
#include <drogon/testing/TestHttpRequest.h>
#include <drogon/testing/TestHttpResponse.h>
#include <drogon/orm/Mapper.h>
#include <drogon/orm/Model.h>
using namespace testing;

class DepartmentsControllerTest : public ::testing::Test {
protected:
    void setupTestServer() {
        auto &app = drogon::app();
        app.addController(std::make_shared<DepartmentsController>());
        app.setLogLevel(drogon::LogLevel::kTrace);
    }

    std::vector<std::string> getValidDepts() {
        std::vector<std::string> deps;
        deps.push_back("{\"id\":1,\"name\":\"HR\"}");
        deps.push_back("{\"id\":2,\"name\":\"Engineering\"}");
        return deps;
    }

    std::vector<std::string> getValidPersons() {
        std::vector<std::string> persons;
        persons.push_back("{\"id\":1,\"department_id\":1,\"name\":\"Alice\"}");
        persons.push_back("{\"id\":2,\"department_id\":1,\"name\":\"Bob\"}");
        return persons;
    }
};

TEST_F(DepartmentsControllerTest, GetDepartments) {
    auto req = TestHttpRequestPtr(new TestHttpRequest("/departments?offset=0&limit=10", "GET", "http://test.com"));
    auto resp = TestHttpResponsePtr(new TestHttpResponse(req));

    // Mock department data
    auto mp = Mapper<Department>(drogon::app().getDbClient().get());
    std::vector<Department> departments;
    departments.push_back(Department{{"HR"}, nullptr, {}});  // Assuming default constructor
    departments.push_back(Department{{"Engineering"}, nullptr, {}});
    
    EXPECT_CALL(mp, findAll(_, testing::_, testing::_, [&](const auto& departments_result){
        EXPECT_EQ(departments_result.size(), 2);
        return departments_result;
    })).WillRepeatedly(testing::Return({departments.begin(), departments.end()}));

    auto controller = std::make_shared<DepartmentsController>();
    controller->get(req, [&](const auto& response) {
        resp->setResponse(std::make_shared<HttpResponse>(response->getHttpServletResponse()));
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
        auto body = resp->getJsonResponse()->toJson();
        EXPECT_TRUE(body.isMember("result"));
        testing::Mock::VerifyAndClearExpectations(&mp);
        done();
    });

    runAllTestsOnce = false;
    EXPECT_TRUE(RunLoop().run());
}

TEST_F(DepartmentsControllerTest, GetDepartmentOne) {
    int deptId = 1;
    auto req = TestHttpRequestPtr(new TestHttpRequest("/departments/{1}/" + std::to_string(deptId), "GET", "http://test.com"));
    auto resp = TestHttpResponsePtr(new TestHttpResponse(req));

    auto mp = Mapper<Department>(drogon::app().getDbClient().get());
    Department dept{{"HR"}, nullptr, {}};
    
    EXPECT_CALL(mp, findByPrimaryKey(testing::_, [&](const auto& result) {
        EXPECT_EQ(result.getId(), deptId);
        return true;  // Simulate successful find
    })).WillOnce(testing::Return(true));

    auto controller = std::make_shared<DepartmentsController>();
    controller->getOne(req, [&](const auto& response) {
        resp->setResponse(std::make_shared<HttpResponse>(response->getHttpServletResponse()));
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k201Created);
        testing::Mock::VerifyAndClearExpectations(&mp);
        done();
    });

    runAllTestsOnce = false;
    EXPECT_TRUE(RunLoop().run());
}

TEST_F(DepartmentsControllerTest, CreateDepartment) {
    Department dept{{"NewDept"}, nullptr, {}};
    auto req = TestHttpRequestPtr(new TestHttpRequest("/departments", "POST", "http://test.com"));
    req->setJsonBody(drogon::Json::parse(drogon::Json::fastToJsonString(dept.toJson())));
    auto resp = TestHttpResponsePtr(new TestHttpResponse(req));

    auto mp = Mapper<Department>(drogon::app().getDbClient().get());
    EXPECT_CALL(mp, insert(testing::_, [&](const auto& result) {
        return true;
    })).WillOnce(testing::Return(true));

    auto controller = std::make_shared<DepartmentsController>();
    controller->createOne(req, [&](const auto& response) {
        resp->setResponse(std::make_shared<HttpResponse>(response->getHttpServletResponse()));
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k201Created);
        testing::Mock::VerifyAndClearExpectations(&mp);
        done();
    });

    runAllTestsOnce = false;
    EXPECT_TRUE(RunLoop().run());
}

TEST_F(DepartmentsControllerTest, UpdateDepartment) {
    int deptId = 1;
    Department dept{{"UpdatedDept"}, nullptr, {}};
    auto req = TestHttpRequestPtr(new TestHttpRequest("/departments/" + std::to_string(deptId), "PUT", "http://test.com"));
    req->setJsonBody(drogon::Json::parse(drogon::Json::fastToJsonString(dept.toJson())));
    auto resp = TestHttpResponsePtr(new TestHttpResponse(req));

    auto mp = Mapper<Department>(drogon::app().getDbClient().get());
    EXPECT_CALL(mp, findFutureByPrimaryKey(testing::_, testing::Invoke([&](int id) {
        Mock::AllowLeaky();
        using namespace std::chrono_literals;
        return std::future<Department>(std::async(std::launch::async, [&] { 
            if (id == deptId) return Department{{"OldDept"}, nullptr, {}};
            throw drogon::orm::DrogonDbException("Not found", 
                std::runtime_error("Resource not found"));
        }));
    })))
    .WillOnce([](int, auto callback) {
        auto future = std::async(std::launch::async, [&] { 
            return Department{{"UpdatedDept"}, nullptr, {}};
        });
        Mock::AllowLeaky();
        (*callback)(future.get());
    });

    auto controller = std::make_shared<DepartmentsController>();
    controller->updateOne(req, [&](const auto& response) {
        resp->setResponse(std::make_shared<HttpResponse>(response->getHttpServletResponse()));
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k204NoContent);
    });

    runAllTestsOnce = false;
    EXPECT_TRUE(RunLoop().run());
}

TEST_F(DepartmentsControllerTest, DeleteDepartment) {
    int deptId = 1;
    auto req = TestHttpRequestPtr(new TestHttpRequest("/departments/" + std::to_string(deptId), "DELETE", "http://test.com"));
    auto resp = TestHttpResponsePtr(new TestHttpResponse(req));

    auto mp = Mapper<Department>(drogon::app().getDbClient().get());
    EXPECT_CALL(mp, deleteBy(_, testing::_, [](const auto& count) { /* Accept any */ }));

    auto controller = std::make_shared<DepartmentsController>();
    controller->deleteOne(req, [&](const auto& response) {
        resp->setResponse(std::make_shared<HttpResponse>(response->getHttpServletResponse()));
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k204NoContent);
        testing::Mock::VerifyAndClearExpectations(&mp);
        done();
    });

    runAllTestsOnce = false;
    EXPECT_TRUE(RunLoop().run());
}

TEST_F(DepartmentsControllerTest, GetDepartmentPersons) {
    int deptId = 1;
    auto req = TestHttpRequestPtr(new TestHttpRequest("/departments/" + std::to_string(deptId) + "/persons", "GET", "http://test.com"));
    auto resp = TestHttpResponsePtr(new TestHttpResponse(req));

    auto mp = Mapper<Department>(drogon::app().getDbClient().get());
    Department dept{{"HR"}, nullptr, {}};
    
    // Mock department existence
    EXPECT_CALL(mp, findFutureByPrimaryKey(testing::_, testing::Invoke([&](int id) {
        if (id == deptId) {
            Mock::AllowLeaky();
            return std::future<Department>(std::async(std::launch::async, [&] { return dept; }));
        }
        throw drogon::orm::DrogonDbException("Not found", std::exception());
    })))
    .WillOnce([](int, auto callback) {
        auto future = std::async(std::launch::async, [&] { 
            Department dept;
            dept.setId(1);
            dept.setName("HR");
            return dept;
        });
        (*callback)(future.get());
    });

    // Mock persons retrieval
    auto personMp = Mapper<Person>(drogon::app().getDbClient().get());
    std::vector<Person> persons = { Person{{"Alice","HR"}},{ Person{{"Bob","HR"}} };
    EXPECT_CALL(personMp, find(_, testing::Criteria().where(Person::Cols::_department_id, CompareOperator::EQ, 1), testing::_, testing::Invoke([&](const auto& results) {
        EXPECT_EQ(results.size(), 2);
        return results;
    })))
    .WillOnce([](auto, auto, auto, auto callback) {
        std::future<std::vector<Person>> future = std::async(std::launch::async, [&] {
            return std::vector<Person>{{{"Alice"}, 1, 1, 1},{{{"Bob"}, 2, 1, 1}};
        });
        (*callback)(future.get());
    });

    auto controller = std::make_shared<DepartmentsController>();
    controller->getDepartmentPersons(req, [&](const auto& response) {
        resp->setResponse(std::make_shared<HttpResponse>(response->getHttpServletResponse()));
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
        testing::Mock::VerifyAllExpectations(&mp, &personMp);
    });

    runAllTestsOnce = false;
    EXPECT_TRUE(RunLoop().run());
}
```