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

class bad_argument_error : public std::runtime_error
{
  using runtime_error::runtime_error;
};

auto parse_args(std::vector<std::string_view> const& args) -> parsed_args
{
  auto parsed_args = ::parsed_args();

  for (auto arg : args) {
    auto index = arg.find('=');
    if (index == std::string_view::npos) {
      auto message = "Bad argument: "s;
      message += arg;
      throw bad_argument_error(message);
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
      auto message = "Option '"s;
      message += type;
      message += "' unknown"sv;
      throw bad_argument_error(message);
    }
  }

  if (parsed_args.input == fs::path()) {
    throw bad_argument_error("'input=<path>' is required");
  }
  if (parsed_args.output == fs::path()) {
    throw bad_argument_error("'output=<path>' is required");
  }

  return parsed_args;
}

auto make_output(parsed_args const& args) -> std::string
{
  auto output = std::stringstream();
  output << "input="sv << args.input << "\noutput="sv << args.output;
  for (auto const& include : args.includes) {
    output << "\ninclude="sv << include;
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
      << "#include <string>\n\nusing namespace std::string_literals;\n\n"
         "namespace\n{\n\nauto const data = R\"prec("sv
      << output << ")prec\"s;\n\n}\n\n"sv
      << std::ifstream(parsed_args.input, std::ios::binary).rdbuf();
}

constexpr int ex_usage = 64;

auto usage() -> int
{
  std::cerr << "Usage: precompiler input=<path> output=<path> "
               "[include=<path>]...\n"sv;
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
  } catch (bad_argument_error const& e) {
    std::cerr << e.what() << '\n';
    return usage();
  } catch (std::exception const& e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  return 0;
}
