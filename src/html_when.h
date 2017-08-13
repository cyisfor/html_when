#include <libxml/HTMLparser.h>
#include <stdbool.h>

void html_when(xmlNode* root);
bool html_when_handled_error(xmlErrorPtr error);
