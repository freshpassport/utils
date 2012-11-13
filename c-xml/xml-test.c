#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

const char *test_file = "/opt/disk/disk-hotreplace.xml";

int main()
{
	xmlDocPtr doc;	// 定义文件指针
	xmlNodePtr rootNode, curNode;	// 节点指针

	xmlChar *szKey;

	doc = xmlReadFile(test_file, "UTF-8", XML_PARSE_RECOVER);

	if (!doc)
		return fprintf(stderr, "加载文件失败!\n");

	// 获取根节点
	rootNode = xmlDocGetRootElement(doc);
	if (!rootNode)
	{
		fprintf(stderr, "获取根节点失败!\n");
		goto error_quit;
	}

	printf("root: %s\n", rootNode->name);

	// 获取节点内容
	curNode = rootNode->xmlChildrenNode;
	while (curNode)
	{
		printf("node: %s\n", curNode->name);
		szKey = xmlNodeGetContent(curNode);
		printf("szKey: %s\n", szKey);
		xmlFree(szKey);
		curNode = curNode->next;
	}

error_quit:
	xmlFreeDoc(doc);
	return -1;
}
