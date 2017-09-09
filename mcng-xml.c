/* 
 * This file is part of mcngd - the Mediacenter Next Generation Daemon
 *   (https://github.com/marioli/loewe-mediacenter-mcngd)
 * Copyright (c) 2017 Mario Lombardo
 * 
 * mcngd is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU Lesser General Public License as   
 * published by the Free Software Foundation, version 3.
 *
 * mcngd is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <libxml/parser.h>

char *mcng_xml_gettoc(const char *filename)
{
	xmlDoc         *document = NULL;
	xmlNode        *root = NULL, *node = NULL, *toc = NULL;
	char *toc_string = NULL;

	if(NULL == (document = xmlReadFile(filename, NULL, XML_PARSE_NOERROR)))
	{
		return NULL;
	}

	root = xmlDocGetRootElement(document);
	if(root && root->type == XML_ELEMENT_NODE)
	{
		node = root->children;
		while(node)
		{
			if(node->type == XML_ELEMENT_NODE && strcmp("toc", (char *)node->name) == 0)
			{
				toc = node->children;
				if(toc->type == XML_TEXT_NODE && toc->content != NULL)
				{
					toc_string = strdup((char *)toc->content);
					break;
				}
			}
			node = node->next;
		}
	}
	xmlFreeDoc(document);

	return toc_string;
}

#if 0
int main(int argc, char **argv)
{
	char *toc;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s filename.xml\n", argv[0]);
		return 1;
	}
	if(NULL != (toc = mcng_xml_gettoc(argv[1])))
	{
		printf("toc: %s\n", toc);
		free(toc);
	}
	return 0;
}
#endif
