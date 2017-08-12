#include <libxml/tree.h> // xmlNode

xmlNode* fuckXPath(xmlNode* parent, const char* name);
xmlNode* fuckXPathDivId(xmlNode* parent, const char* id);
xmlNode* findOrCreate(xmlNode* parent, const char* path);
void foreachNode(xmlNode* parent, const char* name, void (*handle)(xmlNode*,void*), void* ctx);
xmlDoc* readFunky(int fd, const char* content, size_t len);
xmlDoc* strFunky(const char* content, size_t len);
void HTML5_plz(xmlDoc* doc);

bool is_cool_xml_tag(const char* tag, size_t tlen);
void cool_xml_error_handler(void * userData, xmlErrorPtr error);
