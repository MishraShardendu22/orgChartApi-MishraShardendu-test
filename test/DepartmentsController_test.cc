#include <gtest/gtest.h>
#include <drogon/drogon.h>
#include <drogon/http/HttpRequest.h>
#include <drogon/http/HttpResponse.h>
#include <drogon/utils/Utils.h>
#include <drogon/orm/Mapper.h>
#include <drogon/status_code.h>
#include <drogon/drogon_version.h>
#include <json/json.h>
#include <memory>
#include <vector>
#include <string>

using namespace std;
using namespace drogon;
using namespace drogon::orm;
using namespace drogon::http;
using namespace drogon_model::org_chart;

class TestWithMySQL : public ::testing::Test {
protected:
    void SetUp() override {
        app().useClientPool("test_db", 10, "mysql://test_user:test_pass@localhost:3306/test_db", "test_db");
        app().useDbClientPool("test_db", 10, "mysql://test_user:test_pass@localhost:3306/test_db", "test_db");
        database()->executeSql("CREATE TABLE IF NOT EXISTS `department` (" \
        "`id` int(10) unsigned NOT NULL AUTO_INCREMENT, " \
        "`name` varchar(255) NOT NULL, " \
        "PRIMARY KEY (`id`) ) ENGINE=InnoDB DEFAULT CHARSET=utf8;");
        database()->executeSql("INSERT INTO `department` VALUES (1, 'Test Department1'), (2, 'Test Department2')");
    }

    void TearDown() override {
        database()->executeSql("DROP TABLE IF EXISTS department");
    }
};

TEST_F(TestWithMySQL, DepartmentsControllerGetTest) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    req->setPath("/departments");
    req->addOptionalParameter("offset", 0);
    req->addOptionalParameter("limit", 25);
    req->addOptionalParameter("sort_field", "id");
    req->addOptionalParameter("sort_order", "asc");

    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
        auto json = resp->getJsonContent();
        ASSERT_TRUE(json.isArray());
        ASSERT_EQ(json.size(), 2);
    };

    DepartmentsController controller;
    controller.get(req, callback);
}

TEST_F(TestWithMySQL, DepartmentsControllerGetEmptyResponse) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    req->setPath("/departments");
    req->addOptionalParameter("offset", 100);
    req->addOptionalParameter("limit", 25);

    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
        auto json = resp->getJsonContent();
        ASSERT_TRUE(json.isArray());
        EXPECT_TRUE(json.empty());
    };

    DepartmentsController controller;
    controller.get(req, callback);
}

TEST_F(TestWithMySQL, DepartmentsControllerGetOneTest) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    req->setPath("/departments/1");
    
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k201Created);
        auto json = resp->getJsonContent();
        ASSERT_TRUE(json.isObject());
        EXPECT_EQ(json["name"].asString(), "Test Department1");
    };

    DepartmentsController controller;
    controller.getOne(req, callback, 1);
}

TEST_F(TestWithMySQL, DepartmentsControllerGetOneNotFound) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    req->setPath("/departments/999");
    
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k404NotFound);
    };

    DepartmentsController controller;
    controller.getOne(req, callback, 999);
}

TEST_F(TestWithMySQL, DepartmentsControllerCreateOneTest) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    req->setMethod(HttpMethod::Post);
    req->setPath("/departments");

    Department department;
    department.setName("New Department");
    req->setBody(Department::toJson().toJsonString());

    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k201Created);
        auto json = resp->getJsonContent();
        ASSERT_TRUE(json.isObject());
        EXPECT_EQ(json["name"].asString(), "New Department");
    };

    DepartmentsController controller;
    controller.createOne(req, callback, std::move(department));
}

TEST_F(TestWithMySQL, DepartmentsControllerCreateOneValidation) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    req->setMethod(HttpMethod::Post);
    req->setPath("/departments");

    Department department;
    department.setName("");
    req->setBody(Department::toJson().toJsonString());

    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k400BadRequest);
        auto json = resp->getJsonContent();
        ASSERT_TRUE(json.isObject());
        ASSERT_TRUE(json["error"].isString());
    };

    DepartmentsController controller;
    controller.createOne(req, callback, std::move(department));
}

TEST_F(TestWithMySQL, DepartmentsControllerUpdateOneTest) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    req->setMethod(HttpMethod::Put);
    req->setPath("/departments/1");

    Department updatedDepartment;
    updatedDepartment.setId(1);
    updatedDepartment.setName("Updated Department");
    req->setBody(Department::toJson().toJsonString());

    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k204NoContent);
    };

    DepartmentsController controller;
    controller.updateOne(req, callback, 1, std::move(updatedDepartment));
}

TEST_F(TestWithMySQL, DepartmentsControllerUpdateOneNotFound) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    req->setMethod(HttpMethod::Put);
    req->setPath("/departments/999");

    Department updatedDepartment;
    updatedDepartment.setId(999);
    req->setBody(Department::toJson().toJsonString());

    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k404NotFound);
    };

    DepartmentsController controller;
    controller.updateOne(req, callback, 999, std::move(updatedDepartment));
}

TEST_F(TestWithMySQL, DepartmentsControllerDeleteOneTest) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    req->setMethod(HttpMethod::Delete);
    req->setPath("/departments/1");

    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k204NoContent);
    };

    DepartmentsController controller;
    controller.deleteOne(req, callback, 1);
}

TEST_F(TestWithMySQL, DepartmentsControllerDeleteOneNotFound) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    req->setMethod(HttpMethod::Delete);
    req->setPath("/departments/999");

    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k404NotFound);
    };

    DepartmentsController controller;
    controller.deleteOne(req, callback, 999);
}

TEST_F(TestWithMySQL, DepartmentsControllerGetDepartmentPersonsTest) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    req->setPath("/departments/1/persons");
    
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
        auto json = resp->getJsonContent();
        ASSERT_TRUE(json.isArray());
        ASSERT_EQ(json.size(), 0);
    };

    DepartmentsController controller;
    controller.getDepartmentPersons(req, callback, 1);
}

TEST_F(TestWithMySQL, DepartmentsControllerGetDepartmentPersonsError) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    req->setPath("/departments/999/persons");
    
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k404NotFound);
    };

    DepartmentsController controller;
    controller.getDepartmentPersons(req, callback, 999);
}

TEST_F(TestWithMySQL, DepartmentsControllerGetEmptyOffset) {
    HttpRequestPtr req = make_shared<HttpRequest>();
    req->setPath("/departments");
    req->addOptionalParameter("offset", 0);
    
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), HttpStatusCode::k200OK);
        auto json = resp->getJsonContent();
        ASSERT_TRUE(json.isArray());
        ASSERT_EQ(json.size(), 2);
    };

    DepartmentsController controller;
    controller.get(req, callback);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}