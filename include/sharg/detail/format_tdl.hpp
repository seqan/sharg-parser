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

#include <numeric>

#include <sharg/detail/format_base.hpp>
#include <tdl/ParamCTDFile.h>

namespace sharg::detail
{

/*!\brief A generalized format to create different tool description files
 * \ingroup argument_parser
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
        /*CWL, // !TODO in the future this is supposed to also support cwl*/
    };

    //!\brief Vector of functions that stores all calls except add_positional_option.
    std::vector<std::function<void(std::string_view)>> parser_set_up_calls;
    //!\brief Vector of functions that stores add_positional_option calls.
    std::vector<std::function<void()>> positional_option_calls; // singled out to be printed on top
    //!\brief Keeps track of the number of positional options
    unsigned positional_option_count{0};
    //!\brief The names of subcommand programs.
    std::vector<std::string> command_names{};

    //!\brief TDL DS filled with tool meta information
    tdl::ToolInfo info;
    //!\brief TDL DS used to registered all options
    tdl::Param    param;
    //!\brief Targeted tool description format
    FileFormat    fileFormat;

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
    argument_parser_meta_data meta;


public:
    /*!\name Constructors, destructor and assignment
     * \{
     */
    /*!\brief CTor
     *
     * \param fileFormat file format type
     */
    format_tdl(FileFormat fileFormat) : fileFormat{fileFormat} {}
    format_tdl(format_tdl const & pf) = default;             //!< Defaulted.
    format_tdl & operator=(format_tdl const & pf) = default; //!< Defaulted.
    format_tdl(format_tdl &&) = default;                     //!< Defaulted.
    format_tdl & operator=(format_tdl &&) = default;         //!< Defaulted.
    ~format_tdl() = default;                                 //!< Defaulted.


    /*!\brief Prints a section title in man page format to std::cout.
     * \param[in] title The title of the section to print.
     */
    void print_section(std::string const & title)
    {
        param.addSection("nokey", title);
        //std::cout << "Section: " << title << "\n";
    }

    /*!\brief Prints a subsection title in man page format to std::cout.
     * \param[in] title The title of the subsection to print.
     */
    void print_subsection(std::string const & title)
    {
        (void)title;
        //std::cout << "subsection: " << title << "\n";
    }

    /*!\brief Prints a help page section in man page format to std::cout.
     *
     * \param[in] text The text to print.
     * \param[in] line_is_paragraph Whether to insert as paragraph
     *            or just a line (only one line break if not a paragraph).
     */
    void print_line(std::string const & text, bool const line_is_paragraph = true)
    {
        (void)text;
        (void)line_is_paragraph;
        //std::cout << "line: " << text << " " << line_is_paragraph << "\n";
    }



    /*!\brief Prints a help page list_item in man page format to std::cout.
     * \param[in] term The key of the key-value pair of the list item.
     * \param[in] desc The value of the key-value pair of the list item.
     *
     * \details
     *
     * A list item is composed of a key (term) and value (desc)
     * and usually used for option identifier-description-pairs.
     */
    void print_list_item(std::string const & term, std::string const & desc)
    {
        param.setValue(text_only(term), "", text_only(desc));
//        ParamEntry entry{term, desc};

        //std::cout << "list_item: " << text_only(term) << " " << text_only(desc) << "\n";
    }

    /*!\brief removes man page specific formats
     *
     * \param input input string
     * \return string with removed man page specific formats
     */
    std::string text_only(std::string const & input) const
    {
        std::string result;
        for (auto it = input.begin(); it != input.end(); ++it)
        {
            if (*it != '\\')
            {
                result.push_back(*it);
                continue;
            }


            // Handle escape sequence, we interpret only "\-", "\fI", and "\fB".
            ++it;
            assert(!(it == input.end()));
            if (*it == '-')
            {
                result.push_back(*it);
            }
            else if (*it != 'f')
            {
                result.push_back('\\');
                result.push_back(*it);
            }
            else
            {
                ++it;
                assert(!(it == input.end()));
            }
        }

        return result;
    }


    template <typename option_type, typename validator_type>
    void add_option(option_type & value,
                    char const SHARG_DOXYGEN_ONLY(short_id),
                    std::string const & long_id,
                    std::string const & desc,
                    option_spec const spec,
                    validator_type && option_validator)
    {
//        std::string id = long_id + " " + option_type_and_list_info(value);
        std::string info{desc};
        info += ((spec & option_spec::required) ? std::string{" "} : detail::to_string(" Default: ", value, ". "));
        info += option_validator.get_help_page_message();

        auto tags = std::vector<std::string>{};
        if (spec & option_spec::required)
        {
            tags.emplace_back("required");
        }
        if (spec & option_spec::advanced)
        {
            tags.emplace_back("advanced");
        }
        if constexpr (std::same_as<std::filesystem::path, option_type>) {
            //!TODO maybe TDL should support std::filesystem::path
            auto valueAsStr = to_string(value);
            store_help_page_element([this, long_id, info, valueAsStr, tags] (std::string_view app_name) {
                param.setValue(std::string{app_name} + ":--" + long_id, valueAsStr, text_only(info), tags);
            }, spec);
        } else if constexpr (requires { { param.setValue("--" + long_id, value, text_only(info), tags) }; }) {
            store_help_page_element([this, long_id, value, info, tags] (std::string_view app_name) {
                param.setValue(std::string{app_name} + ":--" + long_id, value, text_only(info), tags);
            }, spec);
        } else {
            //!TODO what should we do if the type is unknown?
            store_help_page_element([this, long_id, info, tags] (std::string_view app_name) {
                param.setValue(std::string{app_name} + ":--" + long_id, "", text_only(info), tags);
            }, spec);
        }
    }

    /*!\brief Adds a sharg::print_list_item call to be evaluated later on.
     * \copydetails sharg::argument_parser::add_flag
     */
    void add_flag(bool const & value,
                  char const SHARG_DOXYGEN_ONLY(short_id),
                  std::string const & long_id,
                  std::string const & desc,
                  option_spec const spec)
    {
        store_help_page_element([this, long_id, value, desc] (std::string_view app_name) {
        //!TODO TDL needs support for bools
            param.setValue(std::string{app_name} + ":--" + long_id, value, text_only(desc));
        }, spec);
    }

    /*!\brief Adds a sharg::print_list_item call to be evaluated later on.
     * \copydetails sharg::argument_parser::add_positional_option
     */
    template <typename option_type, typename validator_type>
    void add_positional_option(option_type & value,
                               std::string const & desc,
                               validator_type & option_validator)
    {
        std::string msg = option_validator.get_help_page_message();

        positional_option_calls.push_back([this, &value, desc, msg] ()
        {
            ++positional_option_count;
            auto id   = std::to_string(positional_option_count) + " " + option_type_and_list_info(value);
            auto info = desc +
                            // a list at the end may be empty and thus have a default value
                            ((detail::is_container_option<option_type>)
                                ? detail::to_string(" Default: ", value, ". ")
                                : std::string{" "}) +
                            msg;

            param.setValue(text_only(id), "", text_only(info));
        });
    }


    /*!\brief Initiates the printing of the help page to std::cout.
     * \param[in] parser_meta The meta information that are needed for a detailed help page.
     */
    void parse(argument_parser_meta_data & parser_meta)
    {
        meta = parser_meta;


        if (!command_names.empty())
        {
            //print_section("Subcommands");
            //print_line("This program must be invoked with one of the following subcommands:", false);
            //for (std::string const & name : command_names)
            //    print_line("- \\fB" + name + "\\fP", false);
            //print_line("See the respective help page for further details (e.g. by calling " +
            //                       meta.app_name + " " + command_names[0] + " -h).", true);
            //print_line("The following options below belong to the top-level parser and need to be "
            //           "specified \\fBbefore\\fP the subcommand key word. Every argument after the "
            //           "subcommand key word is passed on to the corresponding sub-parser.", true);
        }
//        param.getEntry(":app-name");


        // add positional options if specified
/*        if (!positional_option_calls.empty())
            print_section("Positional Arguments");*/

        // each call will evaluate the function print_list_item()
        for (auto f : positional_option_calls)
            f();

        // add options and flags if specified
/*        if (!parser_set_up_calls.empty())
            print_section("Options");*/

        // each call will evaluate the function print_list_item()
        for (auto f : parser_set_up_calls)
            f(meta.app_name);

        auto info = tdl::ToolInfo {
            .version_     = meta.version,
            .name_        = meta.app_name,
            .docurl_      = meta.url,
            .category_    = "",
            .description_ = std::accumulate(begin(meta.description), end(meta.description), std::string{}, [](auto a, auto v) {
                return a + v + '\n';
            }),
            .citations_   = {meta.citation},
        };

        if (fileFormat == FileFormat::CTD) {
            tdl::ParamCTDFile file;
            file.writeCTDToStream(&std::cout, param, info);
        } else {
            throw std::runtime_error("unsupported file format (this is a bug)");
        }

        std::exit(EXIT_SUCCESS); // program should not continue from here
    }

    /*!\brief Adds a print_section call to parser_set_up_calls.
     * \copydetails sharg::argument_parser::add_section
     */
    void add_section(std::string const & title, option_spec const spec)
    {
        store_help_page_element([this, title] (std::string_view) { print_section(title); }, spec);
    }

    /*!\brief Adds a print_subsection call to parser_set_up_calls.
     * \copydetails sharg::argument_parser::add_subsection
     */
    void add_subsection(std::string const & title, option_spec const spec)
    {
        store_help_page_element([this, title] (std::string_view app_name) {
            print_subsection(std::string{app_name} + ":" + title); },
        spec);
    }

    /*!\brief Adds a print_line call to parser_set_up_calls.
     * \copydetails sharg::argument_parser::add_line
     */
    void add_line(std::string const & text, bool is_paragraph, option_spec const spec)
    {
        store_help_page_element([this, text, is_paragraph] (std::string_view) {
            print_line(text, is_paragraph);
        }, spec);
    }

    /*!\brief Adds a sharg::print_list_item call to parser_set_up_calls.
     * \copydetails sharg::argument_parser::add_list_item
     */
    void add_list_item(std::string const & key, std::string const & desc, option_spec const spec)
    {
        //!TODO this function shouldn't really exists
        store_help_page_element([this, key, desc] (std::string_view app_name) {
            auto str = text_only(key);
            auto p = str.find(' ');
            if (p != std::string::npos) {
                str = str.substr(0, p);
            }
            str = std::string{app_name} + ":" + str;
            param.setValue(str, "", text_only(desc));
        }, spec);
    }
private:
    /*!\brief Adds a function object to parser_set_up_calls **if** the annotation in `spec` does not prevent it.
     * \param[in] printer The invokable that, if added to `parser_set_up_calls`, prints information to the help page.
     * \param[in] spec The option specification deciding whether to add the information to the help page.
     *
     * \details
     *
     * If `spec` equals `sharg::option_spec::hidden`, the information is never added to the help page.
     */
    void store_help_page_element(std::function<void(std::string_view)> printer, option_spec const spec)
    {
        if (spec & option_spec::hidden) return;
        parser_set_up_calls.push_back(std::move(printer));
    }


};

} // namespace sharg
