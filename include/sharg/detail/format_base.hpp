// SPDX-FileCopyrightText: 2006-2026, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

/*!\file
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 * \brief Provides the format_base struct containing all helper functions
 *        that are needed in all formats.
 */

#pragma once

#include <sharg/auxiliary.hpp>
#include <sharg/config.hpp>
#include <sharg/detail/concept.hpp>
#include <sharg/detail/id_pair.hpp>
#include <sharg/detail/type_name_as_string.hpp>
#include <sharg/validators.hpp>

#if __has_include(<seqan3/version.hpp>)
#    include <seqan3/version.hpp>
#endif

namespace sharg::detail
{

/*!\brief The format that contains all helper functions needed in all formats.
 * \ingroup parser
 * \remark For a complete overview, take a look at \ref parser
 */
class format_base
{
protected:
    /*!\brief Returns the input type as a string (reflection).
     * \tparam value_type The type whose name is converted to std::string.
     * \tparam verbose Whether to use long names ("signed 8 bit integer") or short names ("int8").
     * \returns The type of the value as a string.
     */
    template <typename value_type, bool verbose = true>
    static std::string get_type_name_as_string()
    {
        using type = std::decay_t<value_type>;

        if constexpr (std::is_same_v<type, int8_t>)
            return verbose ? "signed 8 bit integer" : "int8";
        else if constexpr (std::is_same_v<type, uint8_t>)
            return verbose ? "unsigned 8 bit integer" : "uint8";
        else if constexpr (std::is_same_v<type, int16_t>)
            return verbose ? "signed 16 bit integer" : "int16";
        else if constexpr (std::is_same_v<type, uint16_t>)
            return verbose ? "unsigned 16 bit integer" : "uint16";
        else if constexpr (std::is_same_v<type, int32_t>)
            return verbose ? "signed 32 bit integer" : "int32";
        else if constexpr (std::is_same_v<type, uint32_t>)
            return verbose ? "unsigned 32 bit integer" : "uint32";
        else if constexpr (std::is_same_v<type, int64_t>)
            return verbose ? "signed 64 bit integer" : "int64";
        else if constexpr (std::is_same_v<type, uint64_t>)
            return verbose ? "unsigned 64 bit integer" : "uint64";
        else if constexpr (std::is_same_v<type, double>)
            return verbose ? "double" : "double";
        else if constexpr (std::is_same_v<type, float>)
            return verbose ? "float" : "float";
        else if constexpr (std::is_same_v<type, bool>)
            return verbose ? "bool" : "bool";
        else if constexpr (std::is_same_v<type, char>)
            return verbose ? "char" : "char";
        else if constexpr (std::is_same_v<type, std::string>)
            return verbose ? "std::string" : "string";
        else if constexpr (std::is_same_v<type, std::filesystem::path>)
            return verbose ? "std::filesystem::path" : "path";
        else if constexpr (!verbose && std::is_enum_v<type>)
            return "enum";
        else
            return sharg::detail::type_name_as_string<value_type>;
    }

    /*!\brief Returns the `value_type` of the input container as a string (reflection).
     * \tparam container_type The container type for which to query its value_type.
     * \tparam verbose Whether to use long names ("signed 8 bit integer") or short names ("int8").
     * \returns The type of the container's value_type as a string.
     */
    template <detail::is_container_option container_type, bool verbose = true>
    static std::string get_type_name_as_string()
    {
        return get_type_name_as_string<std::ranges::range_value_t<container_type>, verbose>();
    }

    /*!\brief Formats the type of a value for the help page printing.
     * \tparam option_value_type The type of the option value to get the info for.
     * \returns The type of the value as string.
     */
    template <typename option_value_type>
    static std::string option_type_and_list_info(option_value_type const &)
    {
        return ("(\\fI" + get_type_name_as_string<option_value_type>() + "\\fP)");
    }

    /*!\brief Formats the container and its value_type for the help page printing.
     * \tparam container_type A type that must satisfy the sharg::detail::is_container_option.
     *
     * \returns The type of the container value type as a string, encapsulated in "List of".
     */
    template <detail::is_container_option container_type>
    static std::string option_type_and_list_info(container_type const &)
    {
        return ("(\\fIList\\fP of \\fI" + get_type_name_as_string<container_type>() + "\\fP)");
    }

    /*!\brief Formats the option/flag identifier pair for the help page printing.
     * \param[in] id A sharg::detail::id_pair encapsulating both short and long id.
     * \param[in] is_synopsis Whether to use synopsis formatting.
     * \returns The name of the short and long id, prepended with (double)dash.
     *
     * \details
     * For descriptions (`is_synopsis == false`): "-i, --integer", "-i", or "--integer".
     * For synopsis (`is_synopses == true`): "-i|--integer", "-i", or "--integer".
     */
    static std::string prep_id_for_help(detail::id_pair const & id, bool const is_synopsis = false)
    {
        // Build list item term.
        std::string term;

        if (!id.empty_short_id())
            term = "\\fB-" + std::string(1, id.short_id) + "\\fP";

        if (id.has_both_ids())
            is_synopsis ? term.append("|") : term.append(", ");

        if (!id.empty_long_id())
            term.append("\\fB--" + id.long_id + "\\fP");

        return term;
    }

    /*!\brief Escapes certain characters for correct output.
     * \param[in] original The string containing characters to be escaped.
     * \returns The original string as their corresponding html/xml representation.
     *
     * \details Special characters considered are `"`, `\`, `&`, `<` and `>`.
     */
    static std::string escape_special_xml_chars(std::string const & original)
    {
        std::string escaped;
        escaped.reserve(original.size()); // will be at least as long

        for (auto c : original)
        {
            if (c == '"')
                escaped.append("&quot;");
            else if (c == '\'')
                escaped.append("&apos;");
            else if (c == '&')
                escaped.append("&amp;");
            else if (c == '<')
                escaped.append("&lt;");
            else if (c == '>')
                escaped.append("&gt;");
            else
                escaped.push_back(c);
        }

        return escaped;
    }

    /*!\brief Expands multiple one character flag identifiers for pretty help output.
     * \param[in] flag_cluster The string of one character flags.
     * \returns A string that lists all flags as a comma separated list.
     *
     * \details e.g. "-agdg" becomes "-a, -g, -d and -g".
     */
    static std::string expand_multiple_flags(std::string const & flag_cluster)
    {
        std::string tmp;
        auto it{flag_cluster.begin()};

        if (flag_cluster[0] == '-')
            ++it;

        for (; it != flag_cluster.end() - 1; ++it)
            tmp.append({'-', *it, ',', ' '});

        tmp.erase(tmp.find_last_of(',')); // remove last ', '
        tmp.append({'a', 'n', 'd', ' ', '-', flag_cluster[flag_cluster.size() - 1]});

        return tmp;
    }

    /*!\brief Returns the default message for the help page.
     * \tparam option_type The type of the option.
     * \tparam default_type  The type of the default value.
     * \param[in] option The option to get the default message for.
     * \param[in] value The default value to get the default message for.
     * \returns The default message for the help page (" Default: <default-value>. ").
     * \details
     * `value` is either `config.default_message`, or the same as `option`.
     * If the `option_type` is a std::string or std::filesystem::path, the value is quoted.
     * If the `option_type` is a container of std::string or std::filesystem::path, each individual value is quoted;
     * if a `config.default_message` is provided, it will not be quoted.
     */
    template <typename option_type, typename default_type>
    static std::string get_default_message(option_type const & SHARG_DOXYGEN_ONLY(option), default_type const & value)
    {
        static_assert(std::same_as<option_type, default_type> || std::same_as<default_type, std::string>);

        std::stringstream message{};
        message << " Default: ";

        if constexpr (detail::is_container_option<option_type>)
        {
            // If we have a list of strings, we want to quote each string.
            if constexpr (std::same_as<std::ranges::range_value_t<default_type>, std::string>)
            {
                auto view = std::views::transform(value,
                                                  [](auto const & val)
                                                  {
                                                      return std::quoted(val);
                                                  });
                message << detail::to_string(view);
            }
            else // Otherwise we just print the list or the default_message without quotes.
            {
                message << detail::to_string(value);
            }
        }
        else
        {
            static constexpr bool option_is_string = std::same_as<option_type, std::string>;
            static constexpr bool option_is_path = std::same_as<option_type, std::filesystem::path>;
            static constexpr bool value_is_string = std::same_as<default_type, std::string>;

            // Quote: std::string (from value + default_message), and std::filesystem::path (default_message).
            // std::filesystem::path is quoted by the STL's operator<< in detail::to_string.
            static constexpr bool needs_string_quote = option_is_string || (option_is_path && value_is_string);

            if constexpr (needs_string_quote)
                message << std::quoted(value);
            else
                message << detail::to_string(value);
        }

        return message.str();
    }

    /*!\brief Prints a string to std::cout converted to lowercase.
     * \param[in] str The string to print in lowercase.
     * \details
     * This could also be generalized:
     *
     * ```cpp
     * template <std::ranges::input_range range_t>
     * static void print_transform(range_t && range, int (*fun)(int))
     * {
     *     std::ranges::transform(std::forward<range_t>(range),
     *                            std::ostream_iterator<char>(std::cout),
     *                            [fun](unsigned char c)
     *                            {
     *                                 return fun(c);
     *                            });
     * }
     *
     * // Usage:
     * std::string s{"Hello World"};
     * print_transform(s, std::toupper);
     * ```
     *
     * Using `std::function<int(int)> fun` wouldn't work the same way because
     * `std::toupper` also has a templatized overload.
     * For the function pointer, the overload can be infered, from std::function not.
     *
     * ```cpp
     * template <std::ranges::input_range range_t>
     * static void print_transform(range_t && range, std::function<int(int)> fun)
     * {
     *     // ...
     * }
     *
     * // Usage:
     * std::string s{"Hello World"};
     * print_transform(s, static_cast<int(*)(int)>(std::toupper));
     * ```
     *
     * Because `int (*fun)(int)` seems a bit sketchy, and other solutions, like
     * using strong types/enums to decide between upper- and lowercase are more
     * complex, we just have two separate functions.
     */
    static void print_as_lowercase(std::string const & str)
    {
        std::ranges::transform(str,
                               std::ostream_iterator<char>(std::cout),
                               [](unsigned char c)
                               {
                                   return std::tolower(c);
                               });
    }

    /*!\brief Prints a string to std::cout converted to uppercase.
     * \param[in] str The string to print in uppercase.
     * \sa print_as_lowercase
     */
    static void print_as_uppercase(std::string const & str)
    {
        std::ranges::transform(str,
                               std::ostream_iterator<char>(std::cout),
                               [](unsigned char c)
                               {
                                   return std::toupper(c);
                               });
    }
};

/*!\brief The format that contains all helper functions needed in all formats for
 *        printing the interface description of the application (to std::cout).
 * \ingroup parser
 * \remark For a complete overview, take a look at \ref parser
 */
template <typename derived_type>
class format_help_base : public format_base
{
private:
    /*!\name Constructors, destructor and assignment
     * \{
     */
    format_help_base() = default;                                        //!< Defaulted.
    format_help_base(format_help_base const & pf) = default;             //!< Defaulted.
    format_help_base & operator=(format_help_base const & pf) = default; //!< Defaulted.
    format_help_base(format_help_base &&) = default;                     //!< Defaulted.
    format_help_base & operator=(format_help_base &&) = default;         //!< Defaulted.
    ~format_help_base() = default;                                       //!< Defaulted.

    /*!\brief Initializes a format_help_base object.
     * \param[in] names    A list of subcommands (see \link subcommand_parse subcommand parsing \endlink).
     * \param[in] version_updates Whether the developer disabled version checks when constructing the parser.
     * \param[in] advanced Set to `true` to show advanced options.
     */
    format_help_base(std::vector<std::string> const & names,
                     update_notifications const version_updates,
                     bool const advanced) :
        version_check_dev_decision{version_updates},
        command_names{names},
        show_advanced_options{advanced}
    {}
    //!\}

public:
    /*!\brief Adds a sharg::print_list_item call to be evaluated later on.
     * \copydetails sharg::parser::add_option
     */
    template <typename option_type, typename validator_t>
    void add_option(option_type & value, config<validator_t> const & config)
    {
        detail::id_pair const id_pair{config.short_id, config.long_id};
        std::string id = prep_id_for_help(id_pair) + " " + option_type_and_list_info(value);
        std::string info{config.description};

        if (config.default_message.empty())
            info += ((config.required) ? std::string{} : get_default_message(value, value));
        else
            info += get_default_message(value, config.default_message);

        if (auto const & validator_message = config.validator.get_help_page_message(); !validator_message.empty())
            info += ". " + validator_message;

        store_help_page_element(
            [this, id, info]()
            {
                derived_t().print_list_item(id, info);
            },
            config);

        if (!(config.hidden) && (!(config.advanced) || show_advanced_options))
            store_synopsis_option(id_pair,
                                  get_type_name_as_string<option_type, false>(),
                                  config.required,
                                  detail::is_container_option<option_type>);
    }

    /*!\brief Adds a sharg::print_list_item call to be evaluated later on.
     * \copydetails sharg::parser::add_flag
     */
    template <typename validator_t>
    void add_flag(bool & SHARG_DOXYGEN_ONLY(value), config<validator_t> const & config)
    {
        detail::id_pair const id_pair{config.short_id, config.long_id};
        store_help_page_element(
            [this, id = prep_id_for_help(id_pair), description = config.description]()
            {
                derived_t().print_list_item(id, description);
            },
            config);

        // Store for synopsis generation
        if (!(config.hidden) && (!(config.advanced) || show_advanced_options))
            store_synopsis_flag(id_pair);
    }

    /*!\brief Adds a sharg::print_list_item call to be evaluated later on.
     * \copydetails sharg::parser::add_positional_option
     */
    template <typename option_type, typename validator_t>
    void add_positional_option(option_type & value, config<validator_t> const & config)
    {
        // a list at the end may be empty and thus have a default value
        auto positional_default_message = [&value]() -> std::string
        {
            if constexpr (detail::is_container_option<option_type>)
            {
                return get_default_message(value, value);
            }
            else
            {
                (void)value; // Silence unused variable warning.
                return {};
            }
        };

        auto positional_validator_message = [&config]() -> std::string
        {
            if (auto const & validator_message = config.validator.get_help_page_message(); !validator_message.empty())
                return ". " + validator_message;
            else
                return {};
        };

        positional_option_calls.push_back(
            [this,
             &value,
             default_message = positional_default_message(),
             validator_message = positional_validator_message(),
             description = config.description]()
            {
                ++positional_option_count;
                derived_t().print_list_item(detail::to_string("\\fBARGUMENT-",
                                                              positional_option_count,
                                                              "\\fP ",
                                                              option_type_and_list_info(value)),
                                            description + default_message + validator_message);
            });

        // Store for synopsis generation
        store_synopsis_positional(get_type_name_as_string<option_type, false>(),
                                  detail::is_container_option<option_type>);
    }

    /*!\brief Initiates the printing of the help page to std::cout.
     * \param[in] parser_meta The meta information that are needed for a detailed help page.
     * \param[in] executable_name A list of arguments that form together the call to the executable.
     *                            For example: [raptor, build]
     */
    void parse(parser_meta_data & parser_meta, std::vector<std::string> const & executable_name)
    {
        meta = parser_meta;

        derived_t().print_header();

        if (meta.synopsis.empty())
            generate_default_synopsis(executable_name);

        // Synopsis can be disabled by setting `parser.info.synopsis = {""};`
        if (!meta.synopsis.empty() && !meta.synopsis.front().empty())
        {
            derived_t().print_section("Synopsis");
            derived_t().print_synopsis();
        }

        if (!meta.description.empty())
        {
            derived_t().print_section("Description");
            for (auto && desc : meta.description)
                print_line(desc);
        }

        if (!command_names.empty())
        {
            derived_t().print_section("Subcommands");
            derived_t().print_line("This program must be invoked with one of the following subcommands:", false);
            for (std::string const & name : command_names)
                derived_t().print_line("- \\fB" + name + "\\fP", false);
            derived_t().print_line("See the respective help page for further details (e.g. by calling " + meta.app_name
                                       + " " + command_names[0] + " -h).",
                                   true);
            derived_t().print_line("The following options belong to the top-level parser and need to be "
                                   "specified \\fBbefore\\fP the subcommand key word. Every argument after the "
                                   "subcommand key word is passed on to the corresponding sub-parser.",
                                   true);
        }

        // add positional options if specified
        if (!positional_option_calls.empty())
            derived_t().print_section("Positional Arguments");

        // each call will evaluate the function derived_t().print_list_item()
        for (auto && f : positional_option_calls)
            f();

        // There are always options because of the common options
        derived_t().print_section("Options");

        // each call will evaluate the function derived_t().print_list_item()
        for (auto && f : parser_set_up_calls)
            f();

        // print Common options after developer options
        derived_t().print_subsection("Common options");
        derived_t().print_list_item("\\fB-h\\fP, \\fB--help\\fP", "Prints the help page.");
        derived_t().print_list_item("\\fB-hh\\fP, \\fB--advanced-help\\fP",
                                    "Prints the help page including advanced options.");
        derived_t().print_list_item("\\fB--version\\fP", "Prints the version information.");
        derived_t().print_list_item("\\fB--copyright\\fP", "Prints the copyright/license information.");
        derived_t().print_list_item("\\fB--export-help\\fP (std::string)",
                                    "Export the help page information. Value must be one of "
                                        + detail::supported_exports + ".");
        if (version_check_dev_decision == update_notifications::on)
            derived_t().print_list_item("\\fB--version-check\\fP (bool)",
                                        "Whether to check for the newest app version. Default: true");

        if (!meta.examples.empty())
        {
            derived_t().print_section("Examples");
            for (auto && example : meta.examples)
                print_line(example);
        }

        print_version();

        print_legal();

        derived_t().print_footer();
    }

    /*!\brief Adds a print_section call to parser_set_up_calls.
     * \copydetails sharg::parser::add_section
     */
    void add_section(std::string const & title, bool const advanced_only)
    {
        store_help_page_element(
            [this, title]()
            {
                derived_t().print_section(title);
            },
            advanced_only,
            false /* never hidden */);
    }

    /*!\brief Adds a print_subsection call to parser_set_up_calls.
     * \copydetails sharg::parser::add_subsection
     */
    void add_subsection(std::string const & title, bool const advanced_only)
    {
        store_help_page_element(
            [this, title]()
            {
                derived_t().print_subsection(title);
            },
            advanced_only,
            false /* never hidden */);
    }

    /*!\brief Adds a print_line call to parser_set_up_calls.
     * \copydetails sharg::parser::add_line
     */
    void add_line(std::string const & text, bool is_paragraph, bool const advanced_only)
    {
        store_help_page_element(
            [this, text, is_paragraph]()
            {
                derived_t().print_line(text, is_paragraph);
            },
            advanced_only,
            false /* never hidden */);
    }

    /*!\brief Adds a sharg::print_list_item call to parser_set_up_calls.
     * \copydetails sharg::parser::add_list_item
     */
    void add_list_item(std::string const & key, std::string const & desc, bool const advanced_only)
    {
        store_help_page_element(
            [this, key, desc]()
            {
                derived_t().print_list_item(key, desc);
            },
            advanced_only,
            false /* never hidden */);
    }

    /*!\brief Stores all meta information about the application
     *
     * \details
     *
     * This needs to be a member of format_parse, because it needs to present
     * (not filled) when the parser_set_up_calls vector is filled, since all
     * printing functions need some meta information.
     * The member variable itself is filled when copied over from the parser
     * when calling format_parse::parse. That way all the information needed are
     * there, when the actual printing starts.
     *
     * This function is not private because it is needed for short but nicely
     * formatted (error) output to the command line.
     */
    parser_meta_data meta;

    //!\brief Befriend the derived type so it can access private functions.
    friend derived_type;

protected:
    //!\brief Set on construction and indicates whether the developer deactivated the version check calls completely.
    update_notifications version_check_dev_decision{};

    //!\brief Returns the derived type.
    derived_type & derived_t()
    {
        return static_cast<derived_type &>(*this);
    }

    //!\brief Prints a synopsis in any format.
    void print_synopsis()
    {
        constexpr std::string_view end_format_sv = "\\fP";
        for (unsigned i = 0; i < meta.synopsis.size(); ++i)
        {
            std::string text = "\\fB";
            text.append(meta.synopsis[i]);
            // Use iterator syntax instead of index. Index syntax is a segfault if there is no space or tab.
            text.insert(text.begin() + text.find_first_of(" \t"), end_format_sv.begin(), end_format_sv.end());

            derived_t().print_line(text, false);
        }
    }

    /*!\brief Delegates to sharg::print_line(std::string const & text, true) of each format.
     * \param[in] text The text to print.
     */
    void print_line(std::string const & text)
    {
        derived_t().print_line(text, true);
    }

    //!\brief Prints the version information.
    void print_version()
    {
        std::string const version_str{sharg::sharg_version_cstring};

        // Print version, date and url.
        derived_t().print_section("Version");
        derived_t().print_line(derived_t().in_bold("Last update: ") + meta.date, false);
        derived_t().print_line(derived_t().in_bold(meta.app_name + " version: ") + meta.version, false);
        derived_t().print_line(derived_t().in_bold("Sharg version: ") + version_str, false);

#ifdef SEQAN3_VERSION_CSTRING
        std::string const seqan3_version_str{seqan3::seqan3_version_cstring};
        derived_t().print_line(derived_t().in_bold("SeqAn version: ") + seqan3_version_str, false);
#endif

        if (!empty(meta.url))
        {
            derived_t().print_section("Url");
            derived_t().print_line(meta.url, false);
        }
    }

    //!\brief Prints the legal information.
    void print_legal()
    {
        // Print legal stuff
        if ((!empty(meta.short_copyright)) || (!empty(meta.long_copyright)) || (!empty(meta.citation))
            || (!empty(meta.author)) || (!empty(meta.email)))
        {
            derived_t().print_section("Legal");

            if (!empty(meta.short_copyright))
            {
                derived_t().print_line(derived_t().in_bold(meta.app_name + " Copyright: ") + meta.short_copyright,
                                       false);
            }

            if (!empty(meta.author))
            {
                derived_t().print_line(derived_t().in_bold("Author: ") + meta.author, false);
            }

            if (!empty(meta.email))
            {
                derived_t().print_line(derived_t().in_bold("Contact: ") + meta.email, false);
            }

            derived_t().print_line(derived_t().in_bold("SeqAn Copyright: ")
                                       + "2006-2026 Knut Reinert, FU-Berlin; released under the 3-clause BSDL.",
                                   false);

            if (!empty(meta.citation))
            {
                derived_t().print_line(derived_t().in_bold("In your academic works please cite: "), false);
                for (size_t i = 0; i < meta.citation.size(); ++i)
                {
                    // Using `\\fB` and `\\fP` instead of `derived_t().in_bold()` here.
                    // `format_help::print_list_item` uses `format_help::text_width` to determine whether
                    // there should be a new line after the key ("[i]").
                    // `format_help::text_width` ignores special sequences such as `\\fB` and `\\fP`,
                    // but not the actual control sequences produced by `derived_t().in_bold()`.
                    // All formats support `\\fB` and `\\fP`.
                    derived_t().print_list_item("\\fB[" + std::to_string(i + 1u) + "]\\fP", meta.citation[i]);
                }
            }

            if (!empty(meta.long_copyright))
            {
                derived_t().print_line("For full copyright and/or warranty information see "
                                           + derived_t().in_bold("--copyright") + ".",
                                       false);
            }
        }
    }

    //!\brief Generates default synopsis from stored elements.
    void generate_default_synopsis(std::vector<std::string> executable_name)
    {
        if (synopsis_elements.empty())
            return;

        std::string & first = executable_name[0];
        first = first.substr(first.find_last_of('/') + 1);

#ifdef __cpp_lib_ranges_join_with
        auto view = std::views::join_with(executable_name, ' ');
        std::string synopsis_line{view.begin(), view.end()};
#else // libc++ implements std::views::join_with since version 21
        std::string synopsis_line;
        for (size_t i = 0; i < executable_name.size(); ++i)
        {
            if (i > 0)
                synopsis_line += ' ';
            synopsis_line += executable_name[i];
        }
#endif

        std::ranges::stable_sort(synopsis_elements, std::ranges::less{}, &synopsis_element::type);
        auto const pivot = std::ranges::lower_bound(synopsis_elements,
                                                    synopsis_element::option_type::positional,
                                                    std::ranges::less{},
                                                    &synopsis_element::type);

        for (auto it = synopsis_elements.begin(); it != pivot; ++it)
        {
            synopsis_line += " " + it->option_str;
        }
        if (pivot != synopsis_elements.end())
        {
            synopsis_line += " [\\fB--\\fP]";
            for (auto it = pivot; it != synopsis_elements.end(); ++it)
            {
                synopsis_line += " " + it->option_str;
            }
        }

        meta.synopsis.emplace_back(std::move(synopsis_line));
    }

    //!\brief Vector of functions that stores all calls except add_positional_option.
    std::vector<std::function<void()>> parser_set_up_calls;
    //!\brief Vector of functions that stores add_positional_option calls.
    std::vector<std::function<void()>> positional_option_calls; // singled out to be printed on top
    //!\brief Keeps track of the number of positional options
    unsigned positional_option_count{0};
    //!\brief The names of subcommand programs.
    std::vector<std::string> command_names{};
    //!\brief Whether to show advanced options or not.
    bool show_advanced_options{true};
    //!\brief Structure to store synopsis element information.
    struct synopsis_element
    {
        //!\brief Kinds of options.
        enum class option_type : uint8_t
        {
            flag,      //!< Option is a flag.
            option,    //!< Option is a option with argument.
            positional //!< Option is a positional option.
        };
        std::string option_str; //!< The formatted option string.
        option_type type;       //!< Type of the option.
    };

    //!\brief Stores elements for automatic synopsis generation.
    std::vector<synopsis_element> synopsis_elements{};

private:
    /*!\brief Adds a function object to parser_set_up_calls **if** the annotation in `config` does not prevent it.
     * \param[in] printer The invokable that, if added to `parser_set_up_calls`, prints information to the help page.
     * \param[in] advanced Whether the help page element was configured to be hidden.
     * \param[in] hidden Whether the help page element was configured to be hidden.
     *
     * \details
     *
     * If `hidden = true`, the information is never added to the help page.
     * If `advanced = true`, the information is only added to the help page if
     * the advanced help page has been queried on the command line (`show_advanced_options == true`).
     */
    void store_help_page_element(std::function<void()> printer, bool const advanced, bool const hidden)
    {
        if (!(hidden) && (!(advanced) || show_advanced_options))
            parser_set_up_calls.push_back(std::move(printer));
    }

    /*!\brief Stores option information for synopsis generation.
     * \param[in] id A sharg::detail::id_pair encapsulating both short and long id.
     * \param[in] type_str The type string for the option value.
     * \param[in] required Whether the option is required.
     * \param[in] is_list Whether it's a list of arguments.
     * \sa https://pubs.opengroup.org/onlinepubs/9699919799
     */
    void store_synopsis_option(detail::id_pair const & id,
                               std::string const & type_str,
                               bool const required,
                               bool const is_list)
    {
        std::string opt_str = prep_id_for_help(id, true);

        opt_str += " \\fI" + type_str + "\\fP";

        if (!required)
        {
            opt_str = "[" + opt_str + "]";

            if (is_list)
                opt_str.append("...");
        }
        else if (is_list)
        {
            opt_str = opt_str + " [" + opt_str + "]...";
        }

        synopsis_elements.push_back({std::move(opt_str), synopsis_element::option_type::option});
    }

    /*!\brief Stores flag information for synopsis generation.
     * \param[in] id A sharg::detail::id_pair encapsulating both short and long id.
     * \sa https://pubs.opengroup.org/onlinepubs/9699919799
     */
    void store_synopsis_flag(detail::id_pair const & id)
    {
        std::string flag_str = "[" + prep_id_for_help(id, true) + "]";
        synopsis_elements.push_back({std::move(flag_str), synopsis_element::option_type::flag});
    }

    /*!\brief Stores positional argument information for synopsis generation.
     * \param[in] type_str The type string for the positional argument.
     * \param[in] is_list Whether it's a list of arguments.
     * \sa https://pubs.opengroup.org/onlinepubs/9699919799
     */
    void store_synopsis_positional(std::string const & type_str, bool const is_list)
    {
        std::string pos_str = "\\fI" + type_str + "\\fP";

        if (is_list)
            pos_str += "...";

        synopsis_elements.push_back({std::move(pos_str), synopsis_element::option_type::positional});
    }

    /*!\brief Adds a function object to parser_set_up_calls **if** the annotation in `config` does not prevent it.
     * \param[in] printer The invokable that, if added to `parser_set_up_calls`, prints information to the help page.
     * \param[in] config The sharg::config object to access `config.advanced` and  `config.hidden`.
     *
     * \details
     *
     * If `config.hidden = true`, the information is never added to the help page.
     * If `config.advanced = true`, the information is only added to the help page if
     * the advanced help page has been queried on the command line (`show_advanced_options == true`).
     */
    template <typename validator_t>
    void store_help_page_element(std::function<void()> printer, config<validator_t> const & config)
    {
        if (!(config.hidden) && (!(config.advanced) || show_advanced_options))
            parser_set_up_calls.push_back(std::move(printer));
    }
};

} // namespace sharg::detail
