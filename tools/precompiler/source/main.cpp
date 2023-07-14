#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace fs = std::filesystem;

namespace
{

struct parsed_args
{
  fs::path input;
  fs::path output;
  std::vector<fs::path> includes;
};

auto bad_arg = std::make_error_code(std::errc::invalid_argument);

auto parse_args(std::vector<std::string_view> const& args) -> parsed_args
{
  auto parsed_args = ::parsed_args();

  for (auto arg : args) {
    auto index = arg.find('=');
    if (index == std::string_view::npos) {
      throw std::system_error(bad_arg, std::string(arg));
    }

    auto type = arg;
    type.remove_suffix(arg.size() - index);
    arg.remove_prefix(index + 1);

    if (arg.empty()) {
      continue;
    }

    if (type == "input"sv) {
      parsed_args.input = fs::canonical(arg);
    } else if (type == "output"sv) {
      parsed_args.output = fs::absolute(arg);
    } else if (type == "include"sv) {
      parsed_args.includes.emplace_back(fs::canonical(arg));
    } else {
      throw std::system_error(bad_arg, "'" + std::string(type) + "' unknown");
    }
  }

  if (parsed_args.input == fs::path()) {
    throw std::system_error(bad_arg, "'input=<path>' is required");
  }
  if (parsed_args.output == fs::path()) {
    throw std::system_error(bad_arg, "'output=<path>' is required");
  }

  return parsed_args;
}

auto make_output(parsed_args const& args) -> std::string
{
  auto output = std::stringstream();
  output << "input=" << args.input << "\noutput=" << args.output;
  for (const auto& include : args.includes) {
    output << "\ninclude=" << include;
  }
  output << '\n';
  return output.str();
}

void try_main(std::vector<std::string_view> const& args)
{
  auto parsed_args = parse_args(args);
  auto output = make_output(parsed_args);
  std::cerr << output;  // be noisy

  std::ofstream(parsed_args.output, std::ios::binary)
      << "#include <string>\n\nusing namespace "
         "std::string_literals;\n\nnamespace\n{\n\nauto const data = R\"prec("
      << output << ")prec\"s;\n\n}\n\n"
      << std::ifstream(parsed_args.input, std::ios::binary).rdbuf();
}

constexpr int ex_usage = 64;

auto usage() -> int
{
  std::cerr << "Usage: precompiler input=<path> output=<path> "
               "[include=<path>]...\n";
  return ex_usage;
}

}  // namespace

auto main(int argc, char const* argv[]) -> int
{
  try {
    if (argc < 3) {
      return usage();
    }

    auto args = std::vector<std::string_view>();
    args.reserve(static_cast<std::size_t>(argc - 1));
    for (int i = 0; i != argc - 1; ++i) {
      args.emplace_back(argv[i + 1]);
    }

    try_main(args);
  } catch (std::system_error const& e) {
    std::cerr << e.what() << '\n';
    return e.code() == std::errc::invalid_argument ? usage() : 1;
  } catch (std::exception const& e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  return 0;
}
