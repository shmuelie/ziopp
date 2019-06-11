#include <gtest/gtest.h>
#include <ziopp/upath.h>

TEST(UPath, TestAbsoluteAndRelative) {
	ziopp::upath path{ std::string{ "x" } };
	ASSERT_TRUE(path.relative());
	ASSERT_FALSE(path.absolute());

	path = ziopp::upath{ std::string{ ".." } };
	ASSERT_TRUE(path.relative());
	ASSERT_FALSE(path.absolute());

	path = ziopp::upath{ std::string{ "/x" } };
	ASSERT_FALSE(path.relative());
	ASSERT_TRUE(path.absolute());

	path = ziopp::upath{};
	ASSERT_TRUE(path.empty());
}

class TestNormalizeFixture : public ::testing::TestWithParam<std::pair<std::string, std::string>> {
public:
	void SetUp() override {
		auto param = GetParam();
		pathAsText = param.first;
		expectedResult = param.second;
	}
protected:
	std::string pathAsText;
	std::string expectedResult;
};

TEST_P(TestNormalizeFixture, TestNormalize) {
	ziopp::upath path{ pathAsText };
	ASSERT_EQ(expectedResult, path.full_name());

	// Check Equatable
	ziopp::upath expectedPath{ expectedResult };
	ASSERT_TRUE(expectedPath.equals(path));
	ASSERT_TRUE(path == expectedPath);
	ASSERT_FALSE(path != expectedPath);
}

INSTANTIATE_TEST_CASE_P(
	Tests,
	TestNormalizeFixture,
	::testing::Values(
		// Test Empty
		std::make_pair(std::string{ "" }, std::string{ "" }),

		// Tests with regular paths
		std::make_pair(std::string{ "/" }, std::string{ "/" }),
		std::make_pair(std::string{ "\\" }, std::string{ "/" }),
		std::make_pair(std::string{ "a" }, std::string{ "a" }),
		std::make_pair(std::string{ "a/b" }, std::string{ "a/b" }),
		std::make_pair(std::string{ "a\\b" }, std::string{ "a/b" }),
		std::make_pair(std::string{ "a/b/" }, std::string{ "a/b" }),
		std::make_pair(std::string{ "a\\b\\" }, std::string{ "a/b" }),
		std::make_pair(std::string{ "a///b/c//d" }, std::string{ "a/b/c/d" }),
		std::make_pair(std::string{ "///a///b/c//" }, std::string{ "/a/b/c" }),
		std::make_pair(std::string{ "a/b/c" }, std::string{ "a/b/c" }),
		std::make_pair(std::string{ "/a/b" }, std::string{ "/a/b" }),

		// Tests with ".."
		std::make_pair(std::string{ ".." }, std::string{ ".." }),
		std::make_pair(std::string{ "../../a/.." }, std::string{ "../.." }),
		std::make_pair(std::string{ "a/../c" }, std::string{ "c" }),
		std::make_pair(std::string{ "a/b/.." }, std::string{ "a" }),
		std::make_pair(std::string{ "a/b/c/../.." }, std::string{ "a" }),
		std::make_pair(std::string{ "a/b/c/../../.." }, std::string{ "" }),
		std::make_pair(std::string{ "./.." }, std::string{ ".." }),
		std::make_pair(std::string{ "../." }, std::string{ ".." }),
		std::make_pair(std::string{ "../.." }, std::string{ "../.." }),
		std::make_pair(std::string{ "../../" }, std::string{ "../.." }),
		std::make_pair(std::string{ ".a" }, std::string{ ".a" }),
		std::make_pair(std::string{ ".a/b/.." }, std::string{ ".a" }),
		std::make_pair(std::string{ "...a/b../" }, std::string{ "...a/b.." }),
		std::make_pair(std::string{ "...a/.." }, std::string{ "" }),
		std::make_pair(std::string{ "...a/b/.." }, std::string{ "...a" })
	));

class TestCombineFixture : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string>> {
public:
	void SetUp() override {
		auto param = GetParam();
		path1 = std::get<0>(param);
		path2 = std::get<1>(param);
		expectedResult = std::get<2>(param);
	}
protected:
	std::string path1;
	std::string path2;
	std::string expectedResult;
};

TEST_P(TestCombineFixture, TestCombine) {
	auto path = ziopp::upath::combine(path1, path2);
	ASSERT_EQ(expectedResult, (std::string)path);

	path = ziopp::upath{ path1 } / ziopp::upath{ path2 };
	ASSERT_EQ(expectedResult, (std::string)path);

	ziopp::upath expectedPath{ expectedResult };
	ASSERT_TRUE(path.equals(expectedPath));
}

INSTANTIATE_TEST_CASE_P(
	Tests,
	TestCombineFixture,
	::testing::Values(
		std::make_tuple(std::string{ "" }, std::string{ "" }, std::string{ "" }),
		std::make_tuple(std::string{ "/" }, std::string{ "" }, std::string{ "/" }),
		std::make_tuple(std::string{ "\\" }, std::string{ "" }, std::string{ "/" }),
		std::make_tuple(std::string{ "//" }, std::string{ "" }, std::string{ "/" }),
		std::make_tuple(std::string{ "\\\\" }, std::string{ "" }, std::string{ "/" }),
		std::make_tuple(std::string{ "/" }, std::string{ "/" }, std::string{ "/" }),
		std::make_tuple(std::string{ "\\" }, std::string{ "\\" }, std::string{ "/" }),
		std::make_tuple(std::string{ "//" }, std::string{ "//" }, std::string{ "/" }),
		std::make_tuple(std::string{ "" }, std::string{ "/" }, std::string{ "/" }),
		std::make_tuple(std::string{ "a" }, std::string{ "b" }, std::string{ "a/b" }),
		std::make_tuple(std::string{ "a/b" }, std::string{ "c" }, std::string{ "a/b/c" }),
		std::make_tuple(std::string{ "" }, std::string{ "b" }, std::string{ "b" }),
		std::make_tuple(std::string{ "a" }, std::string{ "" }, std::string{ "a" }),
		std::make_tuple(std::string{ "a/b" }, std::string{ "" }, std::string{ "a/b" }),
		std::make_tuple(std::string{ "/a" }, std::string{ "b/" }, std::string{ "/a/b" }),
		std::make_tuple(std::string{ "/a" }, std::string{ "/b" }, std::string{ "/b" }),
		std::make_tuple(std::string{ "/a" }, std::string{ "" }, std::string{ "/a" }),
		std::make_tuple(std::string{ "//a" }, std::string{ "" }, std::string{ "/a" }),
		std::make_tuple(std::string{ "a/" }, std::string{ "" }, std::string{ "a" }),
		std::make_tuple(std::string{ "a//" }, std::string{ "" }, std::string{ "a" }),
		std::make_tuple(std::string{ "a/" }, std::string{ "b" }, std::string{ "a/b" }),
		std::make_tuple(std::string{ "a/" }, std::string{ "b/" }, std::string{ "a/b" }),
		std::make_tuple(std::string{ "a//" }, std::string{ "b//" }, std::string{ "a/b" }),
		std::make_tuple(std::string{ "a" }, std::string{ "../b" }, std::string{ "b" }),
		std::make_tuple(std::string{ "a/../" }, std::string{ "b" }, std::string{ "b" }),
		std::make_tuple(std::string{ "/a/.." }, std::string{ "b" }, std::string{ "/b" }),
		std::make_tuple(std::string{ "/a/.." }, std::string{ "" }, std::string{ "/" }),
		std::make_tuple(std::string{ "//a//..//" }, std::string{ "" }, std::string{ "/" }),
		std::make_tuple(std::string{ "\\a" }, std::string{ "" }, std::string{ "/a" }),
		std::make_tuple(std::string{ "\\\\a" }, std::string{ "" }, std::string{ "/a" })
	));

class TestCombine3Fixture : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string, std::string>> {
public:
	void SetUp() override {
		auto param = GetParam();
		path1 = std::get<0>(param);
		path2 = std::get<1>(param);
		path3 = std::get<2>(param);
		expectedResult = std::get<3>(param);
	}
protected:
	std::string path1;
	std::string path2;
	std::string path3;
	std::string expectedResult;
};

TEST_P(TestCombine3Fixture, TestCombine3) {
	auto path = ziopp::upath::combine(path1, path2, path3);

	ASSERT_EQ(expectedResult, (std::string)path);
	ziopp::upath expectedPath{ expectedResult };
	ASSERT_TRUE(path.equals(expectedPath));;
}

INSTANTIATE_TEST_CASE_P(
	Tests,
	TestCombine3Fixture,
	::testing::Values(
		std::make_tuple(std::string{ "" }, std::string{ "" }, std::string{ "" }, std::string{ "" }),
		std::make_tuple(std::string{ "a" }, std::string{ "b" }, std::string{ "c" }, std::string{ "a/b/c" }),
		std::make_tuple(std::string{ "a/b" }, std::string{ "c" }, std::string{ "d" }, std::string{ "a/b/c/d" }),
		std::make_tuple(std::string{ "" }, std::string{ "b" }, std::string{ "" }, std::string{ "b" }),
		std::make_tuple(std::string{ "a" }, std::string{ "" }, std::string{ "" }, std::string{ "a" }),
		std::make_tuple(std::string{ "a/b" }, std::string{ "" }, std::string{ "" }, std::string{ "a/b" }),
		std::make_tuple(std::string{ "/a" }, std::string{ "b/" }, std::string{ "c/" }, std::string{ "/a/b/c" }),
		std::make_tuple(std::string{ "/a" }, std::string{ "/b" }, std::string{ "/c" }, std::string{ "/c" })
	));