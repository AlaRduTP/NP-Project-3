#ifndef NP_CONSOLE
#define NP_CONSOLE

#include <array>
#include <string>

namespace http_client {

const std::string HTML = ""
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"<meta charset=\"UTF-8\">\n"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
"<title>NP Project 3 Console</title>\n"
"<link\n"
"	href=\"https://fonts.googleapis.com/css?family=Source+Code+Pro\"\n"
"	rel=\"stylesheet\"\n"
"/>\n"
"<link\n"
"	rel=\"icon\"\n"
"	type=\"image/png\"\n"
"	href=\"https://cdn0.iconfinder.com/data/icons/small-n-flat/24/678068-terminal-512.png\"\n"
"/>\n"
"<style>\n"
"\n"
"* {\n"
"	font-family: 'Source Code Pro', monospace;\n"
"	font-size: 1rem !important;\n"
"}\n"
"body {\n"
"	background-color: #212529;\n"
"}\n"
"h3 {\n"
"	color: #cccccc;\n"
"	padding-bottom: 1rem;\n"
"	border-bottom: 1px solid #ccc;\n"
"}\n"
"pre {\n"
"	color: #cccccc;\n"
"}\n"
"b {\n"
"	color: #01b468;\n"
"}\n"
".tab {\n"
"	overflow: hidden;\n"
"	border: 1px solid #ccc;\n"
"	background-color: #f1f1f1;\n"
"}\n"
".tab button {\n"
"	background-color: inherit;\n"
"	float: left;\n"
"	border: none;\n"
"	outline: none;\n"
"	cursor: pointer;\n"
"	padding: 14px 16px;\n"
"	transition: 0.3s;\n"
"	font-size: 17px;\n"
"}\n"
".tab button:hover {\n"
"	background-color: #ddd;\n"
"}\n"
".tab button.active {\n"
"	background-color: #ccc;\n"
"}\n"
".tabcontent {\n"
"	display: none;\n"
"	padding: 6px 12px;\n"
"	border: 1px solid #ccc;\n"
"	border-top: none;\n"
"}\n"
"</style>\n"
"</head>\n"
"<body>\n"
"<div id=\"tabs\" class=\"tab\"></div>\n"
"<div id=\"tabcontents\"></div>\n"
"<script>\n"
"function openTab(evt, cityName) {\n"
"	var i, tabcontent, tablinks;\n"
"	tabcontent = document.getElementsByClassName(\"tabcontent\");\n"
"	for (i = 0; i < tabcontent.length; i++) {\n"
"		tabcontent[i].style.display = \"none\";\n"
"	}\n"
"		tablinks = document.getElementsByClassName(\"tablinks\");\n"
"	for (i = 0; i < tablinks.length; i++) {\n"
"		tablinks[i].className = tablinks[i].className.replace(\" active\", \"\");\n"
"	}\n"
"	document.getElementById(cityName).style.display = \"block\";\n"
"	evt.currentTarget.className += \" active\";\n"
"}\n"
"</script>\n"
"</body>\n"
"</html>\n";

class console {
public:
    explicit console();

    const std::array<std::array<std::string, 3>, 5> & queries();
private:
    std::array<std::array<std::string, 3>, 5> qs_;

    void parse_query_string();
};

}

#endif