// From MorgenGrauen MUDlib
// Rewirtten by lonely for driver of fluffos v2019

#include "break_string.h"

varargs string remove_space(string str, int flag, string dest)
{
    string result;

    if (!stringp(str) || str == "")
        return "";

    if (intp(flag) && flag == TRIM_LEFT)
    {
        result = ltrim(str);
    }
    else if (intp(flag) && flag == TRIM_RIGHT)
    {
        result = rtrim(str);
    }
    else
    {
        result = trim(str);
    }

    if (result != "" && stringp(dest) && dest != "")
        result = replace_string(result, dest, "");

    return result;
}

private string stretch(string str, int width)
{
    int len = strwidth(str);
    string trimmed, *words;
    int start_spaces, word_count, space_count, space_per_word, rest;
    string temp;

    if (len == width) return str;

    trimmed = remove_space(str, TRIM_LEFT);

    if (trimmed == "") return str;

    start_spaces = len - strwidth(trimmed);

    words = explode(trimmed, " ");

    word_count = sizeof(words) - 1;

    if (!word_count)
        return sprintf("%*s", start_spaces, " ") + words[0];

    space_count = width - len;

    space_per_word = (word_count + space_count) / word_count;

    rest = (word_count + space_count) % word_count;

    for (int pos = 0; pos < rest; pos++) words[pos] += " ";

    temp = sprintf("%*s", space_per_word, " ");
    return sprintf("%*s", start_spaces, " ") + implode(words, temp);
}

private varargs string block_string(string str, int width, int flags)
{
    string *tmp;

    if ((flags & BS_LEAVE_MY_LFS) && !(flags & BS_NO_PARINDENT))
    {
        str = " " + replace_string(str, "\n", "\n ", 1);
    }

    str = sprintf("%-*=s", width, str);

    tmp = explode(str, "\n");

    if (sizeof(tmp) > 1)
        return implode(map(tmp[0.. < 2], (: stretch($1, $(width)) :)), "\n") + "\n" + tmp[ < 1];

    return str;
}

varargs string break_string(string str, int width, mixed indent, int flags)
{
    int indentlen, indent2;
    string prefix = "", temp;

    if (!str || str == "") return "";

    if (!width) width = 78;

    if (intp(indent))
    {
        temp = sprintf("%*s", indent, " ");
        indent = indent ? temp : "";
    }

    indentlen = stringp(indent) ? strwidth(indent) : 0;

    if (indentlen > width)
    {
        error(sprintf("break_string: indent longer %d than width %d\n", indentlen, width));
    }

    if (!(flags & BS_LEAVE_MY_LFS))
        str = replace_string(str, "\n", " ", 1);

    if (flags & BS_SINGLE_SPACE)
        str = replace_string(str, "(^|\n| )  *", "\\1", 1);

    if (indentlen && flags & BS_PREPEND_INDENT)
    {
        if (indentlen + strwidth(str) > width || (flags & BS_LEAVE_MY_LFS) && strsrch(str, "\n") > -1)
        {
            prefix = indent + "\n";
            indent = (flags & BS_NO_PARINDENT) ? "" : " ";
            indentlen = strwidth(indent);
        }
    }

    if (flags & BS_BLOCK)
    {
        str = block_string(str, width - indentlen, flags);
    }
    else
    {
        str = sprintf("%-*=s", width - indentlen, str);
    }

    if (str[ < 1] != '\n') str += "\n";

    if (!indentlen) return prefix + str;

    temp = sprintf("%*s", indentlen, " ");
    indent2 = (flags & BS_INDENT_ONCE) ? temp : indent;

    return prefix + indent + replace_string(str[0.. < 2], "\n", "\n" + indent2, 1) + "\n";
}
