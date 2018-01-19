#include <gtest/gtest.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <cassert>
#include <cpr/cpr.h>
#include "httpmockserver/mock_server.h"
#include "httpmockserver/test_environment.h"

/// Port number server is tried to listen on
/// Number is being incremented while free port has not been found.
static const int server_port = 8080;

/// Server started in the main().
static httpmock::TestEnvironment<httpmock::MockServerHolder>* mock_server_env = nullptr;

/// Return URL server is listening at. E.g.: http://localhost:8080/
static std::string getServerUrl() {
    assert(nullptr != mock_server_env);
    const int port = mock_server_env->getMock()->getPort();
    std::ostringstream url;
    url << "http://localhost:" << port << "/";
    return url.str();
}


/// Mock server implementation
class Mock: public httpmock::MockServer {
  public:
    explicit Mock(unsigned port): MockServer(port) {}

    virtual Response responseHandler(
            const std::string &url,
            const std::string &/*method*/,
            const std::string &/*data*/,
            const std::vector<UrlArg> &urlArguments,
            const std::vector<Header> &headers) override
    {
        if (isUrl(url, "/arg_test")) {
            return processArgTest(urlArguments);
        }
        if (isUrl(url, "/header_in")) {
            return processHeaderInTest(headers);
        }
        if (isUrl(url, "/header_out")) {
            return processHeaderOutTest();
        }
        return Response(404, "Page not found.");
    }

  private:
    bool isUrl(const std::string &url, const std::string &urlRequired) const {
        return url.compare(0, urlRequired.size(), urlRequired) == 0;
    }

    /// Process /arg_test request
    Response processArgTest(const std::vector<UrlArg> &urlArguments) const {
        std::vector<UrlArg> sorted = urlArguments;
        std::sort(sorted.begin(), sorted.end(), [](const UrlArg &a, const UrlArg &b) {
            return a.key < b.key;
        });

        std::vector<UrlArg> expected({UrlArg("b", "2"), UrlArg("x", "0")});
        EXPECT_EQ(expected.size(), sorted.size());
        for (size_t i = 0; i < sorted.size(); i++) {
            EXPECT_EQ(expected[i].key, sorted[i].key);
            EXPECT_EQ(expected[i].value, sorted[i].value);
        }
        return Response();
    }

    /// Process /header_in request
    Response processHeaderInTest(const std::vector<Header> &headers) const {
        EXPECT_TRUE(headers.size() > 0);
        bool hasAccept = false;
        for (const Header &header: headers) {
            if (header.key == "accept" && header.value == "application/json") {
                hasAccept = true;
                break;
            }
        }
        EXPECT_TRUE(hasAccept);
        return Response();
    }

    /// Process /header_out request
    Response processHeaderOutTest() const {
        return Response(201, "{}")
            .addHeader({"Content-type", "application/json"});
    }
};


TEST(Server, arguments) {
    const std::string url = getServerUrl();
    cpr::Response r = cpr::Get(cpr::Url{url + "arg_test?x=0&b=2"});
    EXPECT_EQ(200, r.status_code);
}


TEST(Server, headersIn) {
    const std::string url = getServerUrl();
    cpr::Response r = cpr::Get(cpr::Url{url + "header_in"},
                               cpr::Header{{"accept", "application/json"}});
    EXPECT_EQ(200, r.status_code);
}


TEST(Server, headersOut) {
    const std::string url = getServerUrl();
    cpr::Response r = cpr::Get(cpr::Url{url + "header_out"});
    EXPECT_EQ(201, r.status_code);
    EXPECT_EQ(r.header["Content-type"], "application/json");
    EXPECT_EQ(r.text, "{}");
}


TEST(Server, launchFailure) {
    ASSERT_NE(nullptr, mock_server_env);
    const int port = mock_server_env->getMock()->getPort();

    // try to start the server on already used port (with single try)
    EXPECT_THROW({
            httpmock::createMockServerEnvironment<Mock>(port, 1);
    }, std::runtime_error);
}


int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    // startup the server for the tests
    ::testing::Environment * const env = ::testing::AddGlobalTestEnvironment(
            httpmock::createMockServerEnvironment<Mock>(server_port));
    // set global env pointer
    mock_server_env
        = dynamic_cast<httpmock::TestEnvironment<httpmock::MockServerHolder> *>(
            env);
    return RUN_ALL_TESTS();
}
