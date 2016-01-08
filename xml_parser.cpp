#include "stdafx.h"
#include "tools.h"
/*XML */
#include "tinyxml2.h"

tinyxml2::XMLDocument doc;
bool check_null(tinyxml2::XMLElement* element)
{
	if (element == NULL) {
		printf("couldn't find element in XML\n");
		return true;
	}
	return false;
}
void read_xml_doc(char * filename)
{
	doc.LoadFile( filename );
}

const char * get_xml_element_of_element(char * element, char * of_element)
{
	return doc.FirstChildElement(of_element)->FirstChildElement(element)->GetText();
}

const char * get_xml_element_of_element_of_element(char * element, char * of_element, char * root_element)
{
	tinyxml2::XMLElement* root = doc.FirstChildElement();
	tinyxml2::XMLElement* first = root->FirstChildElement(root_element);
	if (!check_null(first)){
		tinyxml2::XMLElement* second = first->FirstChildElement(of_element);
		if (!check_null(second)){
			tinyxml2::XMLElement* third = second->FirstChildElement(element);
			if (!check_null(third)){
				return third->GetText();
			}else
				printf("couldn't find %s in XML\n", element);
		} else
			printf("couldn't find %s in XML\n", of_element);
	} else
		printf("couldn't find %s in XML\n", root_element);
	//printf("couldn't find one(or more) of these elements in XML: %s, %s, %s\n", element, of_element, root_element);
	return NULL;
}
int get_xml_element_attribute(char * element, char * attribute)
{
	int attrib_to_get = 0;
	tinyxml2::XMLElement* attributeapproach = doc.FirstChildElement()->FirstChildElement(element);
	if (attributeapproach == NULL) {
		printf("couldn't find element %s in XML\n", element);
		return 0;
	}
	attributeapproach->QueryIntAttribute( attribute, &attrib_to_get);
	return attrib_to_get;
}