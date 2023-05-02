// -----------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides the format_tdl struct and its helper functions.
 */

#pragma once

#include <concepts>
#include <numeric>

#include <sharg/detail/format_base.hpp>
#include <sharg/validators.hpp>

#include <tdl/ParamDocumentToCTD.h>
#include <tdl/ParamDocumentToCWL.h>

namespace sharg::detail
{

/** \brief converts a value into the corresponding tdl value type
 *
 * \param v value to convert
 * \return returns the matching tdl type
 */
inline auto to_tdl(bool v)
{
    return tdl::BoolValue(v);
}

//!\copydetails sharg::detail::to_tdl
auto to_tdl(std::integral auto v)
{
    return tdl::IntValue(v);
}

//!\copydetails sharg::detail::to_tdl
auto to_tdl(std::floating_point auto v)
{
    return tdl::DoubleValue(v);
}

//!\copydetails sharg::detail::to_tdl
inline auto to_tdl(std::string const & v)
{
    return tdl::StringValue(v);
}

//!\copydetails sharg::detail::to_tdl
auto to_tdl(auto SHARG_DOXYGEN_ONLY(v))
{
    return tdl::BoolValue(false);
}

/*!\brief A generalized format to create different tool description files
 * \ingroup parser
 *
 * \details
 *
 * This class allows to create different outputs format. See FileFormat for
 * available formats.
 */
class format_tdl : format_base
{
public:
    /*!\brief Supported tool description file formats
     */
    enum class FileFormat
    {
        CTD, // Support for CTD format
        CWL, // Support for CWL format
    };

    //!\brief Vector of functions that stores all calls except add_positional_option.
    std::vector<std::function<void(std::string_view)>> parser_set_up_calls;
    //!\brief Vector of functions that stores add_positional_option calls.
    std::vector<std::function<void(std::string_view)>> positional_option_calls; // singled out to be printed on top
    //!\brief Keeps track of the number of positional options
    unsigned positional_option_count{0};
    //!\brief The names of subcommand programs.
    std::vector<std::string> command_names{};

    //!\brief TDL DS filled with tool meta information
    tdl::ToolInfo info;

    //!\brief Targeted tool description format
    FileFormat fileFormat;

    /*!\brief Stores all meta information about the application
     *
     * \details
     *
     * This needs to be a member of format_parse, because it needs to present
     * (not filled) when the parser_set_up_calls vector is filled, since all
     * printing functions need some meta information.
     * The member variable itself is filled when copied over from the argument_parser
     * when calling format_parse::parse. That way all the information needed are
     * there, when the actual printing starts.
     *
     * This function is not private because it is needed for short but nicely
     * formatted (error) output to the command line.
     */
    parser_meta_data meta;

public:
    /*!\name Constructors, destructor and assignment
     * \{
     */
    /*!\brief CTor
     *
     * \param fileFormat file format type
     */
    format_tdl(FileFormat fileFormat) : fileFormat{fileFormat}
    {}

    format_tdl(format_tdl const & pf) = default;             //!< Defaulted.
    format_tdl & operator=(format_tdl const & pf) = default; //!< Defaulted.
    format_tdl(format_tdl &&) = default;                     //!< Defaulted.
    format_tdl & operator=(format_tdl &&) = default;         //!< Defaulted.
    ~format_tdl() = default;                                 //!< Defaulted.

    /*!\brief Adds a sharg::print_list_item call to be evaluated later on.
     * \copydetails sharg::parser::add_option
     */
    template <typename option_type, typename config_type>
    void add_option(option_type & value, config_type const & config)
    {
        auto description = config.description;
        description += (config.required ? std::string{" "} : detail::to_string(" Default: ", value, ". "));
        description += config.validator.get_help_page_message();

        auto tags = std::set<std::string>{};
        if (config.required)
        {
            tags.insert("required");
        }
        if (config.advanced)
        {
            tags.insert("advanced");
        }
        if constexpr (std::same_as<std::filesystem::path, option_type>)
        {
            auto valueAsStr = to_string(value);
            store_help_page_element(
                [this, config, description, valueAsStr, _tags = tags](std::string_view)
                {
                    auto tags = _tags;

                    // Check if validator is a file,directory,input and/or output paremeter
                    using Validator = std::decay_t<decltype(config.validator)>;
                    if constexpr (std::is_base_of_v<sharg::input_file_validator, Validator>)
                    {
                        tags.insert("file");
                    }
                    else if constexpr (std::is_base_of_v<sharg::input_directory_validator, Validator>)
                    {
                        tags.insert("directory");
                    }
                    else if constexpr (std::is_base_of_v<sharg::output_file_validator, Validator>)
                    {
                        tags.insert("file");
                        tags.insert("output");
                    }
                    else if constexpr (std::is_base_of_v<sharg::output_directory_validator, Validator>)
                    {
                        tags.insert("directory");
                        tags.insert("output");
                    }

                    info.params.push_back(tdl::Node{
                        .name = config.long_id,
                        .description = description,
                        .tags = std::move(tags),
                        .value = tdl::StringValue{valueAsStr},
                    });
                    info.cliMapping.emplace_back("--" + config.long_id, config.long_id);
                },
                config);
        }
        else
        {
            store_help_page_element(
                [this, config, value, description, tags](std::string_view)
                {
                    info.params.push_back(tdl::Node{
                        .name = config.long_id,
                        .description = description,
                        .tags = std::move(tags),
                        .value = to_tdl(value),
                    });
                    info.cliMapping.emplace_back("--" + config.long_id, config.long_id);
                },
                config);
        }
    }

    /*!\brief Adds a sharg::print_list_item call to be evaluated later on.
     * \copydetails sharg::parser::add_flag
     */
    template <typename config_type>
    void add_flag(bool & value, config_type const & config)
    {
        store_help_page_element(
            [this, config, value](std::string_view)
            {
                info.params.push_back(tdl::Node{
                    .name = config.long_id,
                    .description = config.description,
                    .tags = {},
                    .value = to_tdl(value),
                });
            },
            config);
    }

    /*!\brief Adds a sharg::print_list_item call to be evaluated later on.
     * \copydetails sharg::parser::add_positional_option
     */
    template <typename option_type, typename config_type>
    void add_positional_option(option_type & value, config_type const & config)
    {
        std::string msg = config.validator.get_help_page_message();

        positional_option_calls.push_back(
            [this, &value, config, msg](std::string_view)
            {
                auto id = "positional_" + std::to_string(positional_option_count);
                ++positional_option_count;
                auto description =
                    config.description +
                    // a list at the end may be empty and thus have a default value
                    ((detail::is_container_option<option_type>) ? detail::to_string(" Default: ", value, ". ")
                                                                : std::string{" "})
                    + msg;

                info.params.push_back(tdl::Node{
                    .name = id,
                    .description = description,
                    .tags = {},
                    .value = tdl::StringValue{},
                });
                if (!config.long_id.empty())
                {
                    info.cliMapping.emplace_back("--" + config.long_id, config.long_id);
                }
            });
    }

    /*!\brief Initiates the printing of the help page to std::cout.
     * \param[in] parser_meta The meta information that are needed for a detailed help page.
     * \param[in] executable_name A list of arguments that form together the call to the executable e.g.: [raptor, build]
     */
    void parse(parser_meta_data & parser_meta, std::vector<std::string> const & executable_name)
    {
        meta = parser_meta;

        // each call will evaluate the function print_list_item()
        for (auto f : positional_option_calls)
            f(meta.app_name);

        // each call will evaluate the function print_list_item()
        for (auto f : parser_set_up_calls)
            f(meta.app_name);

        info.metaInfo = tdl::MetaInfo{
            .version = meta.version,
            .name = meta.app_name,
            .docurl = meta.url,
            .category = "",
            .description = std::accumulate(begin(meta.description),
                                           end(meta.description),
                                           std::string{},
                                           [](auto a, auto v)
                                           {
                                               return a + v + '\n';
                                           }),
            //            .citations   = {meta.citation},
        };
        if (!executable_name.empty())
        {
            info.metaInfo.executableName = executable_name[0];
        }
        for (size_t i{1}; i < executable_name.size(); ++i)
        {
            auto name = "subcommand_" + std::to_string(i);
            info.params.push_back(tdl::Node{
                .name = name,
                .value = tdl::StringValue(executable_name[i]),
            });
            info.cliMapping.emplace_back("", name);
        }

        if (fileFormat == FileFormat::CTD)
        {
            std::cout << tdl::convertToCTD(info);
        }
        else if (fileFormat == FileFormat::CWL)
        {
            std::cout << tdl::convertToCWL(info) << "\n";
        }
        else
        {
            throw std::runtime_error("unsupported file format (this is a bug)");
        }
        std::exit(EXIT_SUCCESS); // program should not continue from here
    }

    /*!\brief Adds a print_section call to parser_set_up_calls.
     * \copydetails sharg::parser::add_section
     */
    void add_section(std::string const & SHARG_DOXYGEN_ONLY(title), bool const SHARG_DOXYGEN_ONLY(advanced_only))
    {}

    /*!\brief Adds a print_subsection call to parser_set_up_calls.
     * \copydetails sharg::parser::add_subsection
     */
    void add_subsection(std::string const & SHARG_DOXYGEN_ONLY(title), bool const SHARG_DOXYGEN_ONLY(advanced_only))
    {}

    /*!\brief Adds a print_line call to parser_set_up_calls.
     * \copydetails sharg::parser::add_line
     */
    void add_line(std::string const & SHARG_DOXYGEN_ONLY(text),
                  bool SHARG_DOXYGEN_ONLY(is_paragraph),
                  bool const SHARG_DOXYGEN_ONLY(advanced_only))
    {}

    /*!\brief Adds a sharg::print_list_item call to parser_set_up_calls.
     * \copydetails sharg::parser::add_list_item
     */
    void add_list_item(std::string const & SHARG_DOXYGEN_ONLY(key),
                       std::string const & SHARG_DOXYGEN_ONLY(desc),
                       bool const SHARG_DOXYGEN_ONLY(advanced_only))
    {}

private:
    /*!\brief Adds a function object to parser_set_up_calls **if** the annotation in `spec` does not prevent it.
     * \param[in] printer The invokable that, if added to `parser_set_up_calls`, prints information to the help page.
     * \param[in] config The option specification deciding whether to add the information to the help page.
     *
     * \details
     *
     * If `spec` equals `sharg::option_spec::hidden`, the information is never added to the help page.
     */
    void store_help_page_element(std::function<void(std::string_view)> printer, config<auto> const & config)
    {
        if (config.hidden)
            return;
        parser_set_up_calls.push_back(std::move(printer));
    }
};

} // namespace sharg::detail
