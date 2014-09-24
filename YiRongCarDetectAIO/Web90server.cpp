#include "stdafx.h"
#include "Web90server.h"

#define WEB_KAKOU_DEBUG 1

void ReadFromFile(char**data,char* pfile)
{
	FILE* fp = fopen(pfile,"rb");
	if(fp)
	{
		fseek(fp,0,SEEK_END);
		unsigned long len=ftell(fp);
		*data=(char *)calloc(len,1);
		fseek(fp,0,SEEK_SET);

		fread(*data,len,1,fp);
		fclose(fp);
	}
	else
	{
		printf("open fail--\r\n");
	}
}

void WriteToFileAB(string buffer,char* pfile)
{
	FILE* fp = fopen(pfile,"ab");
	if(fp)
	{
		fwrite(buffer.data(),1,buffer.size(),fp);
		fclose(fp);
	}
	else
	{
		printf("open fail--\r\n");
	}
}

void WriteToFileWB(string buffer,char* pfile)
{
	FILE* fp = fopen(pfile,"wb");
	if(fp)
	{
		fwrite(buffer.data(),1,buffer.size(),fp);
		fclose(fp);
	}
	else
	{
		printf("open fail--\r\n");
	}
}

/////////////////////////////////////////////////////////////////////////////

void CHAR2UTF8Lite(const char *str,char *utf8)
{
	wchar_t wbufflite[XMLRW_MAX_WSTR];
	// int buffLen = 0;  
	MultiByteToWideChar(CP_ACP, 0, str, -1, wbufflite, XMLRW_MAX_WSTR);  
	// buffLen = WideCharToMultiByte(CP_UTF8, 0, wbuff, -1, NULL, 0, 0, 0);  
	//  utf8 = new char[buffLen+1];  
	WideCharToMultiByte(CP_UTF8, 0, wbufflite, -1, (LPSTR)utf8, XMLRW_MAX_STR, 0, 0);  
}

void UTF82CHARLite(const char *utf8,char *str)
{
	wchar_t wbufflite[XMLRW_MAX_WSTR];
	//	int buffLen = 0;  
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wbufflite, XMLRW_MAX_WSTR);  
	//	buffLen = WideCharToMultiByte(CP_ACP, 0, wbuff, -1, NULL, 0, 0, 0);  
	//	m_gb2312 = new char[buffLen+1];  
	WideCharToMultiByte(CP_ACP, 0, wbufflite, -1, (LPSTR)str, XMLRW_MAX_STR, 0, 0);  
}

void CHAR2UTF8(const char *str,char *utf8,unsigned long int len)
{
	wchar_t * wbuff=(wchar_t *)calloc(len+4,sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str, -1, wbuff, len+2);  
	// buffLen = WideCharToMultiByte(CP_UTF8, 0, wbuff, -1, NULL, 0, 0, 0);  
	//  utf8 = new char[buffLen+1];  
	WideCharToMultiByte(CP_UTF8, 0, wbuff, -1, (LPSTR)utf8, len*2+4, 0, 0); 
	free(wbuff);
}

void UTF82CHAR(const char *utf8,char *str,unsigned long int len)
{
	wchar_t * wbuff=(wchar_t *)calloc(len,sizeof(wchar_t));
	//	int buffLen = 0;  
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wbuff, len+2);  
	//	buffLen = WideCharToMultiByte(CP_ACP, 0, wbuff, -1, NULL, 0, 0, 0);  
	//	m_gb2312 = new char[buffLen+1];  
	WideCharToMultiByte(CP_ACP, 0, wbuff, -1, (LPSTR)str, len*2+4, 0, 0);  
	free(wbuff);
}


/*!
*  \brief ͨ�����ڵ�ͽڵ�����ȡ�ڵ�ָ�롣
*
*  \param pRootEle   xml�ļ��ĸ��ڵ㡣
*  \param strNodeName  Ҫ��ѯ�Ľڵ���
*  \param Node      ��Ҫ��ѯ�Ľڵ�ָ��
*  \return �Ƿ��ҵ���trueΪ�ҵ���Ӧ�ڵ�ָ�룬false��ʾû���ҵ���Ӧ�ڵ�ָ�롣
*/
bool ReadGetNodePointerByName(TiXmlElement* pRootEle,string strNodeName,TiXmlElement* &Node)
{
	// ������ڸ��ڵ��������˳�
	if (strNodeName==pRootEle->Value())
	{
		Node = pRootEle;
		return true;
	}
	TiXmlElement* pEle = pRootEle;  
	for (pEle = pRootEle->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())  
	{  
		//�ݹ鴦���ӽڵ㣬��ȡ�ڵ�ָ��
		if(ReadGetNodePointerByName(pEle,strNodeName,Node))
			return true;
	}  

	return false;
}
/*!
*  \brief ͨ���ڵ��ѯ��
*
*  \param XmlFile   xml�ļ�ȫ·����
*  \param strNodeName  Ҫ��ѯ�Ľڵ��� 
*  \param strText      Ҫ��ѯ�Ľڵ��ı�
*  \return �Ƿ�ɹ���trueΪ�ɹ���false��ʾʧ�ܡ�
//ԭʼ�ڵ�pRootEle
//�ҵ��Ľڵ�FindNode
*/
bool ReadQueryNodeText(TiXmlElement *pRootEle,TiXmlElement* &FindNode,string strNodeName,char *strText,unsigned long int len)
{
	ReadGetNodePointerByName(pRootEle,strNodeName,FindNode);
	if (NULL!=FindNode)
	{
		const char* psz = FindNode->GetText(); 

		if (NULL==psz)
			strText[0] =0;
		else	//ת��
			UTF82CHAR(psz,strText,len);
		return true;
	}
	else
	{
		strText[0] =0;
		return false;
	}
}

bool CreateXMLDataUTF8(TiXmlElement *prow,string attr,const char *valuedata,char *strText,unsigned long int len,bool cdataflag)
{
	// �����ӽڵ㣺data  
	TiXmlElement *pdata = new TiXmlElement(attr);  
	if (NULL==pdata)  
	{  
		return false;  
	}  
	prow->LinkEndChild(pdata);  
	//ת��
	CHAR2UTF8(valuedata,strText,len);
	//����
	TiXmlText *pdataValue = new TiXmlText(strText);  
	pdata->LinkEndChild(pdataValue); 
	//�м�CDATA���
	pdataValue->SetCDATA(cdataflag); 
	return true;
}

bool CreateXMLData(TiXmlElement *prow,string attr,const char *valuedata,bool cdataflag)
{
	// �����ӽڵ㣺data  
	TiXmlElement *pdata = new TiXmlElement(attr);  
	if (NULL==pdata)  
	{  
		return false;  
	}  
	prow->LinkEndChild(pdata);  
	//����
	TiXmlText *pdataValue = new TiXmlText(valuedata);  
	pdata->LinkEndChild(pdataValue); 
	pdataValue->SetCDATA(cdataflag); 
	return true;
}
/*
<?xml version="1.0" encoding="UTF-8"?>
<soap:Envelope xmlns:soap="http://www.w3.org/2003/05/soap-envelope" xmlns:ser="http://service.thirdBayonet.webservice.bms.hikvision.com">
<soap:Header/>
<soap:Body>
<ser:initSystem>
<ser:xml>

<![CDATA[

<loginParam> 
<cmsUrl>35.24.13.26</cmsUrl> 
<userName>admin</userName> 
<passwd>fzga12345</passwd> 
</loginParam>

]]>


</ser:xml>
</ser:initSystem>
</soap:Body>
</soap:Envelope>
*/
bool CreateXml(char *methodname,const char *valuedata,
			   char *strText,unsigned long int len)  
{  
	// ����һ��TiXmlDocument��ָ��  
	TiXmlDocument pDoc;  

	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0","utf-8","");  
	if (NULL==pDeclaration)  
	{  
		return false;  
	}  

	pDoc.LinkEndChild(pDeclaration);  
	// ����һ�����ڵ㣺pRootEle 
	TiXmlElement *pRootEle = new TiXmlElement("soapenv:Envelope");  
	if (NULL==pRootEle)  
	{  
		return false;  
	}  
	pDoc.LinkEndChild(pRootEle);  
	pRootEle->SetAttribute("xmlns:soap","http://www.w3.org/2003/05/soap-envelope");
	pRootEle->SetAttribute("xmlns:ser", "http://service.thirdBayonet.webservice.bms.hikvision.com");

	// �����ӽڵ㣺pHeader
	TiXmlElement *pHeader = new TiXmlElement("soap:Header"); 
	if (NULL==pHeader)  
	{  
		return false;  
	}  
	pRootEle->LinkEndChild(pHeader);  

	// �����ӽڵ㣺pbody
	TiXmlElement *pbody = new TiXmlElement("soap:Body"); 
	if (NULL==pbody)  
	{  
		return false;  
	}  
	pRootEle->LinkEndChild(pbody);  


	// �����ӽڵ㣺row
	TiXmlElement *pmethod = new TiXmlElement(methodname);  
	if (NULL==pmethod)  
	{  
		return false;  
	}  
	pbody->LinkEndChild(pmethod);  

	//���� ���� ������ַ��Ѿ���UTF8 ���ﲻ����ת
	if(!CreateXMLData(pmethod,
		"ser:xml",
		valuedata,
		true))
		return false;

	//���浽�ַ���
	TiXmlPrinter printer;
	pDoc.Accept(&printer);
	//	strResult.Format("%s", printer.CStr());
	strcpy(strText,printer.CStr());

	if(WEB_KAKOU_DEBUG)
	{
		printf("xml=\n%s\n",strText);
		//�����ļ�
		pDoc.SaveFile("d:\\1.xml");
	}
	return true;  
}   

//����CDATA����  ��������
bool CreateXmlLite(char *name,struct NAME_VALUE_S mapdata[],int maplen,
							  char *strText,unsigned long int len)  
{  
	// ����һ��TiXmlDocument��ָ��  
	TiXmlDocument pDoc;  

	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0","utf-8","");  
	if (NULL==pDeclaration)  
	{  
		return false;  
	}  

	pDoc.LinkEndChild(pDeclaration);  
	// ����һ�����ڵ㣺pRootEle 
	TiXmlElement *pRootEle = new TiXmlElement(name);  
	if (NULL==pRootEle)  
	{  
		return false;  
	}  
	pDoc.LinkEndChild(pRootEle);  

	int it;
	for(it=0;it<maplen;it++)
	{

		if(!CreateXMLDataUTF8(pRootEle,
			mapdata[it].namestr.c_str(),
			mapdata[it].valuestr.c_str(),
			strText,len,
			false))
			return false;

	}

	//�����ļ�
	//pDoc.SaveFile("d:\\1.xml");

	//���浽�ַ���
	TiXmlPrinter printer;
	pDoc.Accept(&printer);
	//	strResult.Format("%s", printer.CStr());
	strcpy(strText,printer.CStr());

	if(WEB_KAKOU_DEBUG)
		printf("xml=\n%s\n",strText);

	return true;  
}   
/*
bool CreateXmlLite_initSystem(char *ip,char *username,char *psw,
			   char *strText,unsigned long int len)  
{  
	// ����һ��TiXmlDocument��ָ��  
	TiXmlDocument pDoc;  

	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0","utf-8","");  
	if (NULL==pDeclaration)  
	{  
		return false;  
	}  

	pDoc.LinkEndChild(pDeclaration);  
	// ����һ�����ڵ㣺pRootEle 
	TiXmlElement *pRootEle = new TiXmlElement("loginParam");  
	if (NULL==pRootEle)  
	{  
		return false;  
	}  
	pDoc.LinkEndChild(pRootEle);  
	
	if(!CreateXMLDataUTF8(pRootEle,
		"cmsUrl",
		ip,
		strText,len,
		false))
		return false;

	if(!CreateXMLDataUTF8(pRootEle,
		"userName",
		username,
		strText,len,
		false))
		return false;

	if(!CreateXMLDataUTF8(pRootEle,
		"passwd",
		psw,
		strText,len,
		false))
		return false;

	//�����ļ�
	//pDoc.SaveFile("d:\\1.xml");

	//���浽�ַ���
	TiXmlPrinter printer;
	pDoc.Accept(&printer);
	//	strResult.Format("%s", printer.CStr());
	strcpy(strText,printer.CStr());

	if(WEB_KAKOU_DEBUG)
		printf("xml=\n%s\n",strText);

	return true;  
}   
*/


bool SendSoap_InitSystem(char *wsdlUrl,char *ip,char *username,char *psw,
					  char *failstr)
{
	Service CService;

	CService.InitData();

	char xmlstr[51200];
	char xmllitestr[51200];

	char soapactionstr[512]="Content-Type: application/soap+xml;charset=UTF-8;action=\"urn:initSystem\"";
/*
	if(false==CreateXmlLite_initSystem(ip,username,psw,
		xmllitestr,51200))
	{
		strcpy(failstr,"SendPidPushSoap �޷�����XML lite�ļ�");
		printf("SendPidPushSoap �޷�����XML lite�ļ�\n");

		return false;
	}
*/
	/*
	map<string ,string> mapdata;
	mapdata.insert(pair<string,string>("cmsUrl",ip));
	mapdata.insert(pair<string,string>("userName",username));
	mapdata.insert(pair<string,string>("passwd",psw));
*/

 ;

	struct NAME_VALUE_S mapdata[3];
	mapdata[0].i("cmsUrl",ip);
	mapdata[1].i("userName",username);
	mapdata[2].i("passwd",psw);

	if(false==CreateXmlLite("loginParam",mapdata,3,
		xmllitestr,51200))
	{
		strcpy(failstr,"SendSoap_InitSystem �޷�����XML lite�ļ�");
		printf("SendSoap_InitSystem �޷�����XML lite�ļ�\n");

		return false;
	}

	if(false==CreateXml("ser:initSystem",
		xmllitestr,
		xmlstr,51200))
	{
		strcpy(failstr,"SendSoap_InitSystem �޷�����XML�ļ�");
		printf("SendSoap_InitSystem �޷�����XML�ļ�\n");

		return false;
	}

#if 0
	int nRes = CService.PostSoap(wsdlUrl,soapactionstr,xmlstr,failstr);

	if(0!=nRes)
	{
		strcat(failstr," SendInitSystemSoap POST����");
		printf("SendInitSystemSoap POST����\n");
		CService.DestroyData();
		return false;
	}

	if(WEB_KAKOU_DEBUG)
		printf("\nSendInitSystemSoap resp_buffer=%s\n",CService.m_resp_buffer.c_str());
/*
	if(NULL!=strstr(CService.m_resp_buffer.c_str(),"<ns:return>"))
	{
		strcat(failstr," SendPidPushSoap ���ر��Ĳ�Ϊ0");
		CService.DestroyData();
		return false;
	}
	*/

	//����Ĭ��UTF-8��
	char* charstr=(char *)calloc(CService.m_resp_buffer.length()*2+4,sizeof(char));

	UTF82CHAR(CService.m_resp_buffer.c_str(),charstr,CService.m_resp_buffer.length()*2);

	//��ֹ���Ĵ��� �������һ��<���ϴ���ȥ���н���
	const char *presp=strchr(charstr,'<');
	if(NULL== presp)
	{
		strcpy(failstr,"SendPidPushSoap XML���� �Ҳ���<");
		printf("SendPidPushSoap XML���� �Ҳ���<\n");
		free(charstr);
		CService.DestroyData();
		return false;
	}
	
	char* utf8=(char *)calloc(strlen(presp)*2+4,sizeof(char));
	
	CHAR2UTF8(presp,utf8,strlen(presp)*2);

	//����ջ�� �ڴ治��й¶
	TiXmlDocument myDocument;
	myDocument.Parse(utf8);

	free(utf8);
	free(charstr);

	TiXmlElement *pRootEle = myDocument.RootElement();
	if (NULL==pRootEle)
	{
		strcpy(failstr,"SendPidPushSoap XML����");
		printf("SendPidPushSoap XML����\n");
		CService.DestroyData();
		return false;
	}

	char temp[XMLRW_MAX_STR]="";

	TiXmlElement *pNodeRow = NULL;
	TiXmlElement *pNodeData = NULL;

	for(pNodeRow = pRootEle; pNodeRow; pNodeRow = pNodeRow->NextSiblingElement())
	{
		pNodeData = NULL;
		if(ReadQueryNodeText(pNodeRow,pNodeData,"ns:return",temp,XMLRW_MAX_STR))
		{
			printf("ns:return=%s\n",temp);
		}
	}

	if(0!=strcmp(temp,"0000"))
	{
		sprintf(failstr,"���س���:%s",temp);
		printf("%s\n",failstr);
		CService.DestroyData();
		return false;
	}
#endif

	CService.DestroyData();

	return true;

}




bool SendSoap_insertCrossingInfo(char *wsdlUrl,
								 char *id,char *crossindex,char *crossname,char *longi, char *lati,
								char *failstr)
{
	Service CService;

	CService.InitData();

	char xmlstr[51200];
	char xmllitestr[51200];

	char soapactionstr[512]="Content-Type: application/soap+xml;charset=UTF-8;action=\"urn:insertCrossingInfo\"";
	/*
	map<string ,string> mapdata;

	mapdata.insert(pair<string,string>("sessionId",id));
	mapdata.insert(pair<string,string>("crossingIndex",crossindex));
	mapdata.insert(pair<string,string>("crossingName",crossname));
	mapdata.insert(pair<string,string>("longitude",longi));
	mapdata.insert(pair<string,string>("latitude",lati));
	mapdata.insert(pair<string,string>("drivewayNum","0"));
	mapdata.insert(pair<string,string>("controlindexCode","0"));
	*/

	struct NAME_VALUE_S mapdata[7];

	mapdata[0].i("sessionId",id);
	mapdata[1].i("crossingIndex",crossindex);
	mapdata[2].i("crossingName",crossname);
	mapdata[3].i("longitude",longi);
	mapdata[4].i("latitude",lati);
	mapdata[5].i("drivewayNum","0");
	mapdata[6].i("controlindexCode","0");

	if(false==CreateXmlLite("crossingInfo",mapdata,7,
		xmllitestr,51200))
	{
		strcpy(failstr,"SendSoap_insertCrossingInfo �޷�����XML lite�ļ�");
		printf("SendSoap_insertCrossingInfo �޷�����XML lite�ļ�\n");

		return false;
	}

	if(false==CreateXml("ser:insertCrossingInfo",
		xmllitestr,
		xmlstr,51200))
	{
		strcpy(failstr,"SendSoap_insertCrossingInfo �޷�����XML�ļ�");
		printf("SendSoap_insertCrossingInfo �޷�����XML�ļ�\n");

		return false;
	}

#if 0
	int nRes = CService.PostSoap(wsdlUrl,soapactionstr,xmlstr,failstr);

	if(0!=nRes)
	{
		strcat(failstr," SendInitSystemSoap POST����");
		printf("SendInitSystemSoap POST����\n");
		CService.DestroyData();
		return false;
	}

	if(WEB_KAKOU_DEBUG)
		printf("\nSendInitSystemSoap resp_buffer=%s\n",CService.m_resp_buffer.c_str());
/*
	if(NULL!=strstr(CService.m_resp_buffer.c_str(),"<ns:return>"))
	{
		strcat(failstr," SendPidPushSoap ���ر��Ĳ�Ϊ0");
		CService.DestroyData();
		return false;
	}
	*/

	//����Ĭ��UTF-8��
	char* charstr=(char *)calloc(CService.m_resp_buffer.length()*2+4,sizeof(char));

	UTF82CHAR(CService.m_resp_buffer.c_str(),charstr,CService.m_resp_buffer.length()*2);

	//��ֹ���Ĵ��� �������һ��<���ϴ���ȥ���н���
	const char *presp=strchr(charstr,'<');
	if(NULL== presp)
	{
		strcpy(failstr,"SendPidPushSoap XML���� �Ҳ���<");
		printf("SendPidPushSoap XML���� �Ҳ���<\n");
		free(charstr);
		CService.DestroyData();
		return false;
	}
	
	char* utf8=(char *)calloc(strlen(presp)*2+4,sizeof(char));
	
	CHAR2UTF8(presp,utf8,strlen(presp)*2);

	//����ջ�� �ڴ治��й¶
	TiXmlDocument myDocument;
	myDocument.Parse(utf8);

	free(utf8);
	free(charstr);

	TiXmlElement *pRootEle = myDocument.RootElement();
	if (NULL==pRootEle)
	{
		strcpy(failstr,"SendPidPushSoap XML����");
		printf("SendPidPushSoap XML����\n");
		CService.DestroyData();
		return false;
	}

	char temp[XMLRW_MAX_STR]="";

	TiXmlElement *pNodeRow = NULL;
	TiXmlElement *pNodeData = NULL;

	for(pNodeRow = pRootEle; pNodeRow; pNodeRow = pNodeRow->NextSiblingElement())
	{
		pNodeData = NULL;
		if(ReadQueryNodeText(pNodeRow,pNodeData,"ns:return",temp,XMLRW_MAX_STR))
		{
			printf("ns:return=%s\n",temp);
		}
	}

	if(0!=strcmp(temp,"0000"))
	{
		sprintf(failstr,"���س���:%s",temp);
		printf("%s\n",failstr);
		CService.DestroyData();
		return false;
	}
#endif

	CService.DestroyData();

	return true;

}


#if 0
bool SendPidPushSoap(char *wsdlUrl,char *targetNamespace,char *methodname,
					 char *valuename,const char *valuedata,
					  char *failstr)
{
	Service CService;

	CService.InitData();

	char xmlstr[51200];
	//
	char methodnametemp[256];
	char valuenametemp[256];

	//char targetNamespace[256]="http://push.services.webService.vm.yr.com";

	char soapactionstr[256];

	//sprintf(methodnametemp,"push:%s","concentratePushSend");
	//sprintf(valuenametemp,"push:%s","args0");

	sprintf(methodnametemp,"push:%s",methodname);
	sprintf(valuenametemp,"push:%s",valuename);

	sprintf(soapactionstr,"SOAPAction: \"%s/%s\"",targetNamespace,methodname);

	//ת��BASE64����
	char *base64str=(char *)calloc(OUT_ENBASE64_SIZE(strlen(valuedata))+10,sizeof(char));
	base64_encode(base64str,(OUT_ENBASE64_SIZE(strlen(valuedata))+10)*sizeof(char),(unsigned char *)valuedata,strlen(valuedata));

	if(false==CreateXml(targetNamespace,
		methodnametemp,
		valuenametemp,
		//valuedata,
		base64str,
		xmlstr,51200))
	{
		strcpy(failstr,"SendPidPushSoap �޷�����XML�ļ�");
		printf("SendPidPushSoap �޷�����XML�ļ�\n");

		free(base64str);
		return false;
	}

	free(base64str);

//	int nRes = CService.PostSoap("http://10.142.50.20:8080/uvss/services/infoPushSend",
	//	"SOAPAction: \"http://push.services.webService.vm.yr.com/concentratePushSend\"",
	//	xmlstr);


	int nRes = CService.PostSoap(wsdlUrl,soapactionstr,xmlstr,failstr);

	if(0!=nRes)
	{
		strcat(failstr," SendPidPushSoap POST����");
		printf("SendPidPushSoap POST����\n");
		CService.DestroyData();
		return false;
	}

	if(WEB_KAKOU_DEBUG)
		printf("\nSendPidPushSoap resp_buffer=%s\n",CService.m_resp_buffer.c_str());
/*
	if(NULL!=strstr(CService.m_resp_buffer.c_str(),"<ns:return>"))
	{
		strcat(failstr," SendPidPushSoap ���ر��Ĳ�Ϊ0");
		CService.DestroyData();
		return false;
	}
	*/
	
	//����Ĭ��UTF-8��
	char* charstr=(char *)calloc(CService.m_resp_buffer.length()*2+4,sizeof(char));

	UTF82CHAR(CService.m_resp_buffer.c_str(),charstr,CService.m_resp_buffer.length()*2);

	//��ֹ���Ĵ��� �������һ��<���ϴ���ȥ���н���
	const char *presp=strchr(charstr,'<');
	if(NULL== presp)
	{
		strcpy(failstr,"SendPidPushSoap XML���� �Ҳ���<");
		printf("SendPidPushSoap XML���� �Ҳ���<\n");
		free(charstr);
		CService.DestroyData();
		return false;
	}
	
	char* utf8=(char *)calloc(strlen(presp)*2+4,sizeof(char));
	
	CHAR2UTF8(presp,utf8,strlen(presp)*2);

	//����ջ�� �ڴ治��й¶
	TiXmlDocument myDocument;
	myDocument.Parse(utf8);

	free(utf8);
	free(charstr);

	TiXmlElement *pRootEle = myDocument.RootElement();
	if (NULL==pRootEle)
	{
		strcpy(failstr,"SendPidPushSoap XML����");
		printf("SendPidPushSoap XML����\n");
		CService.DestroyData();
		return false;
	}

	char temp[XMLRW_MAX_STR]="";

	TiXmlElement *pNodeRow = NULL;
	TiXmlElement *pNodeData = NULL;

	for(pNodeRow = pRootEle; pNodeRow; pNodeRow = pNodeRow->NextSiblingElement())
	{
		pNodeData = NULL;
		if(ReadQueryNodeText(pNodeRow,pNodeData,"ns:return",temp,XMLRW_MAX_STR))
		{
			printf("ns:return=%s\n",temp);
		}
	}

	if(0!=strcmp(temp,"0000"))
	{
		sprintf(failstr,"���س���:%s",temp);
		printf("%s\n",failstr);
		CService.DestroyData();
		return false;
	}

	CService.DestroyData();

	return true;

}

#endif