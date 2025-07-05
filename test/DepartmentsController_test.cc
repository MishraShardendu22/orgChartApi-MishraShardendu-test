CMakeLists.txt:
```cmake
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(org_chart_tests)

enable_language(CXX)

# Includes
include(FindPackageHandleDependentOptions)
include(SelectCompileDefinitions)
include(GNUInstallDirs)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/test
    ${DROGON_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${UUID_INCLUDE_DIRS}
    ${BROTLI_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIRS}
    ${TRANTOR_INCLUDE_DIRS}
    ${JSONCONS_INCLUDE_DIRS}
)

# Link directories
link_directories(
    ${DROGON_LIBRARY_DIRS}
    ${JSONCPP_LIBRARY_DIRS}
    ${UUID_LIBRARY_DIRS}
    ${BROTLI_LIBRARY_DIRS}
    ${OPENSSL_LIBRARY_DIRS}
    ${TRANTOR_LIBRARY_DIRS}
)

# Add the test executable
add_executable(org_chart_tests test_controllers.cc test_main.cc)

# Link libraries
target_link_libraries(org_chart_tests
    ${DROGON_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    ${UUID_LIBRARIES}
    ${BROTLI_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${TRANTOR_LIBRARIES}
    gtest
    pthread
)

# Optional: Add custom commands for running tests
add_custom_target(test_run
    DEPENDS org_chart_tests
    COMMAND $<TARGET_FILE:org_chart_tests>
    COMMENT "Running unit tests"
)

test_controllers.cc:
```cpp
#include <gtest/gtest.h>
#include <drogon/HttpController.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpRequest.h>
#include <drogon/orm/Model.h>
#include <drogon/utils/Log.h>
#include <vector>
#include <memory>
#include <utility>
#include <string>

using namespace drogon;
using namespace drogon_model::org_chart;

class DepartmentsControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize drogon logger for tests
        LOG(INFO) << "Setting up test environment";
        
        // Setup any necessary mock objects if needed
        // For simple controller tests we might not need mocks
    }
};

TEST_F(DepartmentsControllerTest, GetDepartmentsWithParameters) {
    DepartmentsController controller;
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const HttpResponsePtr &)> callback = [](const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k200OK);
    };

    req->setParameter("offset", "0");
    req->setParameter("limit", "10");
    req->setParameter("sort_field", "name");
    req->setParameter("sort_order", "desc");

    controller.get(req, callback);
}

TEST_F(DepartmentsControllerTest, GetDepartmentsWithoutParameters) {
    DepartmentsController controller;
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const HttpResponsePtr &)> callback = [](const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k200OK);
    };

    controller.get(req, callback);
}

TEST_F(DepartmentsControllerTest, GetOneValidDepartment) {
    DepartmentsController controller;
    int departmentId = 1;
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const HttpResponsePtr &)> callback = [](const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k201Created);
    };

    req->setPath("/departments/1");
    controller.getOne(req, callback, departmentId);
}

TEST_F(DepartmentsControllerTest, CreateOneDepartment) {
    DepartmentsController controller;
    Department department;
    // Setup department data
    department.setName("Engineering");
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const HttpResponsePtr &)> callback = [](const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k201Created);
    };

    controller.createOne(req, callback, std::move(department));
}

TEST_F(DepartmentsControllerTest, UpdateOneDepartment) {
    DepartmentsController controller;
    int departmentId = 1;
    Department dept;
    dept.setName("Updated Engineering");
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const HttpResponsePtr &)> callback = [](const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k204NoContent);
    };

    controller.updateOne(req, callback, departmentId, std::move(dept));
}

TEST_F(DepartmentsControllerTest, DeleteOneDepartment) {
    DepartmentsController controller;
    int departmentId = 1;
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const HttpResponsePtr &)> callback = [](const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k204NoContent);
    };

    controller.deleteOne(req, callback, departmentId);
}

TEST_F(DepartmentsControllerTest, GetDepartmentPersons) {
    DepartmentsController controller;
    int departmentId = 1;
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const HttpResponsePtr &)> callback = [](const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k200OK);
    };

    controller.getDepartmentPersons(req, callback, departmentId);
}

TEST_F(DepartmentsControllerTest, GetOneNonExistentDepartment) {
    DepartmentsController controller;
    int departmentId = 999;
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const HttpResponsePtr &)> callback = [](const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k404NotFound);
    };

    controller.getOne(req, callback, departmentId);
}

TEST_F(DepartmentsControllerTest, UpdateNonExistentDepartment) {
    DepartmentsController controller;
    int departmentId = 999;
    Department dept;
    dept.setName("New Engineering");
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const HttpResponsePtr &)> callback = [](const auto &resp) {
        auto status = resp->getStatusCode();
        auto body = resp->getJSONValue();
        ASSERT_EQ(status, HttpStatusCode::k404NotFound) << "Status code is 404";
    };

    controller.updateOne(req, callback, departmentId, std::move(dept));
}

TEST_F(DepartmentsControllerTest, DeleteNonExistentDepartment) {
    DepartmentsController controller;
    int departmentId = 999;
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const HttpResponsePtr &)> callback = [](const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k404NotFound);
    };

    controller.deleteOne(req, callback, departmentId);
}

TEST_F(DepartmentsControllerTest, DatabaseErrorInGet) {
    // This would be more complex in real tests, requires mocking the DB client
    DepartmentsController controller;
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const HttpResponsePtr &)> callback = [](const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k500InternalServerError);
    };

    // Setup mocks to simulate database error
    // Normally, one would mock the Mapper's findAll method to throw an exception here
    controller.get(req, callback);
}

TEST_F(DepartmentsControllerTest, DatabaseErrorInGetOne) {
    DepartmentsController controller;
    int departmentId = 1;
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const HttpResponsePtr &)> callback = [](const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k500InternalServerError);
    };

    // Setup mocks to simulate database error
    controller.getOne(req, callback, departmentId);
}

TEST_F(DepartmentsControllerTest, DatabaseErrorInCreateOne) {
    DepartmentsController controller;
    Department department;
    department.setName("Testing");
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const HttpResponsePtr &)> callback = [](const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k500InternalServerError);
    };

    // Setup mocks to simulate database error
    controller.createOne(req, callback, std::move(department));
}

TEST_F(DepartmentsControllerTest, DatabaseErrorInUpdateOne) {
    DepartmentsController controller;
    int departmentId = 1;
    Department department;
    department.setName("Testing");
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k500InternalServerError);
    };

    controller.updateOne(req, callback, departmentId, std::move(department));
}

TEST_F(DepartmentsControllerTest, DatabaseErrorInDeleteOne) {
    DepartmentsController controller;
    int departmentId = 1;
    auto req = std::make_shared<HttpRequest>();
    std::function<void(const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k500InternalServerError);
    };

    controller.deleteOne(req, callback, departmentId);
}

TEST_F(DepartmentsControllerTest, DatabaseErrorInGetDepartmentPersons) {
    DepartmentsController controller;
    int departmentId = 1;
    auto req = std::shared_ptr<HttpRequest>();
    std::function<void(const auto &resp) {
        auto status = resp->getStatusCode();
        ASSERT_EQ(status, HttpStatusCode::k500InternalServerError);
    };

    controller.getDepartmentPersons(req, callback, departmentId);
        
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```