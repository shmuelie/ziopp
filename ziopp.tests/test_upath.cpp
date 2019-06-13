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

	ASSERT_EQ(path.full_name(), path.to_absolute().full_name());

	path = ziopp::upath{};
	ASSERT_TRUE(path.empty());
}

#define TEST_STRING_PAIR_FIXTURE(NAME, PARAMA, PARAMB) class NAME ## Fixture : public ::testing::TestWithParam<std::pair<std::string, std::string>> { \
public: \
void SetUp() override { \
	auto param = GetParam(); \
	PARAMA = param.first; \
	PARAMB = param.second; \
} \
protected: \
	std::string PARAMA; \
	std::string PARAMB; \
}

TEST_STRING_PAIR_FIXTURE(TestNormalize, pathAsText, expectedResult);

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
		std::make_pair(std::string{}, std::string{}),

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
		std::make_pair(std::string{ "a/b/c/../../.." }, std::string{}),
		std::make_pair(std::string{ "./.." }, std::string{ ".." }),
		std::make_pair(std::string{ "../." }, std::string{ ".." }),
		std::make_pair(std::string{ "../.." }, std::string{ "../.." }),
		std::make_pair(std::string{ "../../" }, std::string{ "../.." }),
		std::make_pair(std::string{ ".a" }, std::string{ ".a" }),
		std::make_pair(std::string{ ".a/b/.." }, std::string{ ".a" }),
		std::make_pair(std::string{ "...a/b../" }, std::string{ "...a/b.." }),
		std::make_pair(std::string{ "...a/.." }, std::string{}),
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
		std::make_tuple(std::string{}, std::string{}, std::string{}),
		std::make_tuple(std::string{ "/" }, std::string{}, std::string{ "/" }),
		std::make_tuple(std::string{ "\\" }, std::string{}, std::string{ "/" }),
		std::make_tuple(std::string{ "//" }, std::string{}, std::string{ "/" }),
		std::make_tuple(std::string{ "\\\\" }, std::string{}, std::string{ "/" }),
		std::make_tuple(std::string{ "/" }, std::string{ "/" }, std::string{ "/" }),
		std::make_tuple(std::string{ "\\" }, std::string{ "\\" }, std::string{ "/" }),
		std::make_tuple(std::string{ "//" }, std::string{ "//" }, std::string{ "/" }),
		std::make_tuple(std::string{}, std::string{ "/" }, std::string{ "/" }),
		std::make_tuple(std::string{ "a" }, std::string{ "b" }, std::string{ "a/b" }),
		std::make_tuple(std::string{ "a/b" }, std::string{ "c" }, std::string{ "a/b/c" }),
		std::make_tuple(std::string{}, std::string{ "b" }, std::string{ "b" }),
		std::make_tuple(std::string{ "a" }, std::string{}, std::string{ "a" }),
		std::make_tuple(std::string{ "a/b" }, std::string{}, std::string{ "a/b" }),
		std::make_tuple(std::string{ "/a" }, std::string{ "b/" }, std::string{ "/a/b" }),
		std::make_tuple(std::string{ "/a" }, std::string{ "/b" }, std::string{ "/b" }),
		std::make_tuple(std::string{ "/a" }, std::string{}, std::string{ "/a" }),
		std::make_tuple(std::string{ "//a" }, std::string{}, std::string{ "/a" }),
		std::make_tuple(std::string{ "a/" }, std::string{}, std::string{ "a" }),
		std::make_tuple(std::string{ "a//" }, std::string{}, std::string{ "a" }),
		std::make_tuple(std::string{ "a/" }, std::string{ "b" }, std::string{ "a/b" }),
		std::make_tuple(std::string{ "a/" }, std::string{ "b/" }, std::string{ "a/b" }),
		std::make_tuple(std::string{ "a//" }, std::string{ "b//" }, std::string{ "a/b" }),
		std::make_tuple(std::string{ "a" }, std::string{ "../b" }, std::string{ "b" }),
		std::make_tuple(std::string{ "a/../" }, std::string{ "b" }, std::string{ "b" }),
		std::make_tuple(std::string{ "/a/.." }, std::string{ "b" }, std::string{ "/b" }),
		std::make_tuple(std::string{ "/a/.." }, std::string{}, std::string{ "/" }),
		std::make_tuple(std::string{ "//a//..//" }, std::string{}, std::string{ "/" }),
		std::make_tuple(std::string{ "\\a" }, std::string{}, std::string{ "/a" }),
		std::make_tuple(std::string{ "\\\\a" }, std::string{}, std::string{ "/a" })
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
	ASSERT_TRUE(path.equals(expectedPath));
}

INSTANTIATE_TEST_CASE_P(
	Tests,
	TestCombine3Fixture,
	::testing::Values(
		std::make_tuple(std::string{}, std::string{}, std::string{}, std::string{}),
		std::make_tuple(std::string{ "a" }, std::string{ "b" }, std::string{ "c" }, std::string{ "a/b/c" }),
		std::make_tuple(std::string{ "a/b" }, std::string{ "c" }, std::string{ "d" }, std::string{ "a/b/c/d" }),
		std::make_tuple(std::string{}, std::string{ "b" }, std::string{}, std::string{ "b" }),
		std::make_tuple(std::string{ "a" }, std::string{}, std::string{}, std::string{ "a" }),
		std::make_tuple(std::string{ "a/b" }, std::string{}, std::string{}, std::string{ "a/b" }),
		std::make_tuple(std::string{ "/a" }, std::string{ "b/" }, std::string{ "c/" }, std::string{ "/a/b/c" }),
		std::make_tuple(std::string{ "/a" }, std::string{ "/b" }, std::string{ "/c" }, std::string{ "/c" })
	));

TEST_STRING_PAIR_FIXTURE(TestGetDirectory, path1, expectedDir);

TEST_P(TestGetDirectoryFixture, TestGetDirectory) {
	ziopp::upath path{path1};
	const ziopp::upath result = path.directory();
	ASSERT_EQ(expectedDir, result.full_name());
}

INSTANTIATE_TEST_CASE_P(
	Tests,
	TestGetDirectoryFixture,
	::testing::Values(
		std::make_pair(std::string{}, std::string{}),
		std::make_pair(std::string{"/a"}, std::string{"/"}),
		std::make_pair(std::string{"/a/b"}, std::string{"/a"}),
		std::make_pair(std::string{"/a/b/c.txt"}, std::string{"/a/b"}),
		std::make_pair(std::string{"a"}, std::string{}),
		std::make_pair(std::string{"../a"}, std::string{".."}),
		std::make_pair(std::string{"../../a/b"}, std::string{"../../a"})
	));

class TestInDirectoryFixture : public ::testing::TestWithParam<std::tuple<std::string, std::string, bool, bool>>
{
public:
	void SetUp() override
	{
		auto param = GetParam();
		path1 = std::get<0>(param);
		directory = std::get<1>(param);
		recursive = std::get<2>(param);
		expected = std::get<3>(param);
	}
protected:
	std::string path1;
	std::string directory;
	bool recursive;
	bool expected;
};

TEST_P(TestInDirectoryFixture, TestInDirectory) {
	ziopp::upath path{path1};
	bool result = path.in_directory(directory, recursive);
	ASSERT_EQ(expected, result);
}

INSTANTIATE_TEST_CASE_P(
	Tests,
	TestInDirectoryFixture,
	::testing::Values(
		// Test automatic separator insertion
        std::make_tuple(std::string{ "/a/b/c" }, std::string{ "/a/b" }, false, true),
        std::make_tuple(std::string{ "/a/bc" }, std::string{ "/a/b" }, false, false),

        // Test trailing separator
        std::make_tuple(std::string{ "/a/b/" }, std::string{ "/a" }, false, true),
        std::make_tuple(std::string{ "/a/b" }, std::string{ "/a/" }, false, true),
        std::make_tuple(std::string{ "/a/b/" }, std::string{ "/a/" }, false, true),

        // Test recursive option
        std::make_tuple(std::string{ "/a/b/c" }, std::string{ "/a" }, true, true),
        std::make_tuple(std::string{ "/a/b/c" }, std::string{ "/a" }, false, false),

        // Test relative paths
        std::make_tuple(std::string{ "a/b" }, std::string{ "a" }, false, true),

        // Test exact match
        std::make_tuple(std::string{ "/a/b/" }, std::string{ "/a/b/" }, false, true),
        std::make_tuple(std::string{ "/a/b/" }, std::string{ "/a/b/" }, true, true),
        std::make_tuple(std::string{ "/a/b" }, std::string{ "/a/b" }, false, true),
        std::make_tuple(std::string{ "/a/b" }, std::string{ "/a/b" }, true, true)
	));

TEST_STRING_PAIR_FIXTURE(TestName, path1, expectedName);

TEST_P(TestNameFixture, TestName) {
	ziopp::upath path{ path1 };
	const std::string result = path.name();
	ASSERT_EQ(expectedName, result);
}

INSTANTIATE_TEST_CASE_P(
	Tests,
	TestNameFixture,
	::testing::Values(
		std::make_pair(std::string{}, std::string{}),
		std::make_pair(std::string{ "/" }, std::string{}),
		std::make_pair(std::string{ "/a" }, std::string{ "a" }),
		std::make_pair(std::string{ "/a/b" }, std::string{ "b" }),
		std::make_pair(std::string{ "/a/b/c.txt" }, std::string{ "c.txt" }),
		std::make_pair(std::string{ "a" }, std::string{ "a" }),
		std::make_pair(std::string{ "../a" }, std::string{ "a" }),
		std::make_pair(std::string{ "../../a/b" }, std::string{ "b" })
	));

TEST_STRING_PAIR_FIXTURE(TestNameWithoutExtension, path1, expectedName);

TEST_P(TestNameWithoutExtensionFixture, TestNameWithoutExtension) {
	ziopp::upath path{ path1 };
	const std::string result = path.name_without_extension();
	ASSERT_EQ(expectedName, result);
}

INSTANTIATE_TEST_CASE_P(
	Tests,
	TestNameWithoutExtensionFixture,
	::testing::Values(
		std::make_pair(std::string{ "" }, std::string{ "" }),
		std::make_pair(std::string{ "/" }, std::string{ "" }),
		std::make_pair(std::string{ "/a" }, std::string{ "a" }),
		std::make_pair(std::string{ "/a/b" }, std::string{ "b" }),
		std::make_pair(std::string{ "/a/b/c.txt" }, std::string{ "c" }),
		std::make_pair(std::string{ "a" }, std::string{ "a" }),
		std::make_pair(std::string{ "../a" }, std::string{ "a" }),
		std::make_pair(std::string{ "../../a/b" }, std::string{ "b" })
	));