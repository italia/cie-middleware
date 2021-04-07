//
//  LdapCrl.m
//  iDigitalSApp
//
//  Created by svp on 02/04/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//
#include "ASN1/UUCByteArray.h"

#include "LdapCrl.h"
#ifdef WIN32
#include "Winldap.h"
#include "Winber.h"

#pragma comment(lib, "Wldap32.lib")
#else
//#include <ldap.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32

long getCRLFromLDAP(char* url, UUCByteArray& data)
{
//	int              i;
//	int              err;
//	int              ver;
//	char           * attribute;
//	LDAP           * ld;
//	BerValue         cred;
//	BerValue       * servercredp;
//	BerElement     * ber;
//	char		   * dn;
//	char		   * h;
//	char           host[256];
//	char           * attributes;
//	char           * scope;
//	char           * extensions;
//	char           * filter;
//
//	LDAPMessage    * res = NULL;
//	LDAPMessage    * entry;
//	struct berval ** vals;
//
//	vals            = NULL;
//	servercredp     = NULL;
//	cred.bv_val     = NULL;//"drowssap";
//	cred.bv_len     = 0;//(size_t) strlen("drowssap");
//
//	h = strtok(url, "://");
//
//	sprintf(host, "%s://%s", h, strtok(NULL, "/"));
//
//	dn = strtok(NULL, "?");
//
//	/*
//	NSString* decodedDN = [[NSString stringWithCString:dn] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
//	dn = [decodedDN cString];
//	*/
//
//	attributes = strtok(NULL, "?");
//	scope = strtok(NULL, "?");
//	filter = strtok(NULL, "?");
//	extensions = strtok(NULL, "?");
//
//	if(attributes != NULL)
//	{
//		for(i = 0; attributes[i]; i++)
//			 attributes[i] = tolower(attributes[i]);
//	}
//	else
//	{
//		attribute = "certificaterevocationlist";
//	}
//
//	ld = ldap_init(host, 389);
//	if (!ld)
//	{
//		//NSLog(@"ldap_initialize(): %s", ldap_err2string(err));
//		return LdapGetLastError();
//	};
//
//	ver = LDAP_VERSION3;
//	err = ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &ver);
//	if (err != LDAP_SUCCESS)
//	{
//		//NSLog(@"ldap_set_option(): %s", ldap_err2string(err));
//		ldap_unbind_s(ld);
//		return err;
//	};
//
//	//NSLog(@"binding to LDAP server...");
//
//	//err = ldap_sasl_bind_s(ld, "", LDAP_AUTH_SIMPLE, &cred, NULL, NULL, NULL);
//	err = ldap_bind_s(ld, "", "", LDAP_AUTH_SIMPLE);
//	if (err != LDAP_SUCCESS)
//	{
//		//NSLog(@"ldap_sasl_bind_s(): %s", ldap_err2string(err));
//		ldap_unbind_s(ld);
//		return err;
//	};
//
//	//NSLog(@"initiating lookup...");
//	if ((err = ldap_search_ext_s(ld, dn, LDAP_SCOPE_BASE, filter, NULL, 0, NULL, NULL, NULL, -1, &res)))
//	{
//		//NSLog(@"ldap_search_ext_s(): %s", ldap_err2string(err));
//		if(res != NULL)
//			ldap_msgfree(res);
//
//		ldap_unbind_s(ld);
//		return err;
//	};
//
//	//NSLog(@"checking for results...");
//	if (!(ldap_count_entries(ld, res)))
//	{
//		//NSLog(@"no entries found.");
//		ldap_msgfree(res);
//		ldap_unbind_s(ld);
//		return 0;
//	};
//
//	//NSLog(@"%i entries found.", ldap_count_entries(ld, res));
//
//	//NSLog(@"retrieving results...");
//	if (!(entry = ldap_first_entry(ld, res)))
//	{
//		//NSLog(@"ldap_first_entry(): %s", ldap_err2string(err));
//		ldap_msgfree(res);
//		ldap_unbind_s(ld);
//		return 0;
//	};
//
//	while(entry)
//	{
//		//NSLog(@" ");
//		//NSLog(@"dn: %s", ldap_get_dn(ld, entry));
//
//		attribute = ldap_first_attribute(ld, entry, &ber);
//		while(attribute)
//		{
//			if ((vals = ldap_get_values_len(ld, entry, attribute)))
//			{
//				//for(i = 0; vals[i]; i++)
//				//	NSLog(@"%s: %s", attribute, vals[i]->bv_val);
//
//
//				for(i = 0; attributes[i]; i++)
//					attribute[i] = tolower(attribute[i]);
//
//				char* szAttribute = strtok(attribute, ";");
//
//				if(strcmp(szAttribute, attributes) == 0)
//				{
//					data.append((BYTE*)vals[0]->bv_val, vals[0]->bv_len);
//
//					ldap_value_free_len(vals);
//					ldap_memfree(attribute);
//					ldap_msgfree(res);
//					ber_free(ber, 0);
//					ldap_unbind_s(ld);
//
//					return 0;
//				};
//
//				ldap_value_free_len(vals);
//				ldap_memfree(attribute);
//			}
//			attribute = ldap_next_attribute(ld, entry, ber);
//		};
//
//		ber_free(ber, 0);
//
//		// skip to the next entry
//		entry = ldap_next_entry(ld, entry);
//	};
//
//	//NSLog(@" ");
//
//	//NSLog(@"unbinding from LDAP server...");
//	ldap_msgfree(res);
//	ldap_unbind_s(ld);
//
	return 0;
}
#else
/*
long getCRLFromLDAP(const char* url, UUCByteArray& data)
{
	int              i;
	int              err;
	int              ver;
	char           * attribute;
	LDAP           * ld;
	BerValue         cred;
	BerValue       * servercredp;
	BerElement     * ber;
	char		   * dn;
	char		   * h;
	char           host[256];
	char           * attributes;
	char           * scope;
	char           * extensions;
	char           * filter;
	
	LDAPMessage    * res = NULL;
	LDAPMessage    * entry;
	struct berval ** vals;
	
	vals            = NULL;
	servercredp     = NULL;
	cred.bv_val     = NULL;//"drowssap";
	cred.bv_len     = 0;//(size_t) strlen("drowssap");
	
	h = strtok(url, "://");
	//h = strtok(NULL, "/");
	
	
	sprintf(host, "%s://%s", h, strtok(NULL, "/")); 
	
	dn = strtok(NULL, "?");

	NSString* decodedDN = [[NSString stringWithCString:dn] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];

	
	dn = [decodedDN cString];
	
	
	attributes = strtok(NULL, "?");
	scope = strtok(NULL, "?");
	filter = strtok(NULL, "?");
	extensions = strtok(NULL, "?");
	
	if(attributes != NULL)
	{
		NSString* sattr = [[NSString stringWithCString:attributes] lowercaseString];
	
		attributes = [sattr cString];
	}
	else 
	{
		attribute = "certificaterevocationlist";
	}

	
	//NSLog(@"initialzing LDAP...");
	err = ldap_initialize(&ld, host);
	if (err != LDAP_SUCCESS)
	{
		NSLog(@"ldap_initialize(): %s", ldap_err2string(err));
		return nil;
	};
	
	ver = LDAP_VERSION3;
	err = ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &ver);
	if (err != LDAP_SUCCESS)
	{
		NSLog(@"ldap_set_option(): %s", ldap_err2string(err));
		ldap_unbind_ext_s(ld, NULL, NULL);
		return nil;
	};
	
	//NSLog(@"binding to LDAP server...");
	
	err = ldap_sasl_bind_s(ld, "", LDAP_SASL_SIMPLE, &cred, NULL, NULL, NULL);
	//err = ldap_bind_s(ld, dn, &cred, LDAP_AUTH_SIMPLE);
	if (err != LDAP_SUCCESS)
	{
		NSLog(@"ldap_sasl_bind_s(): %s", ldap_err2string(err));
		ldap_unbind_ext_s(ld, NULL, NULL);
		return nil;
	};
	
	//NSLog(@"initiating lookup...");
	if ((err = ldap_search_ext_s(ld, dn, LDAP_SCOPE_BASE, filter, NULL, 0, NULL, NULL, NULL, -1, &res)))
	{
		NSLog(@"ldap_search_ext_s(): %s", ldap_err2string(err));
		if(res != NULL)
			ldap_msgfree(res);
		ldap_unbind_ext_s(ld, NULL, NULL);
		return nil;
	};
	
	//NSLog(@"checking for results...");
	if (!(ldap_count_entries(ld, res)))
	{
		NSLog(@"no entries found.");
		ldap_msgfree(res);
		ldap_unbind_ext_s(ld, NULL, NULL);
		return nil;
	};
	//NSLog(@"%i entries found.", ldap_count_entries(ld, res));
	
	//NSLog(@"retrieving results...");
	if (!(entry = ldap_first_entry(ld, res)))
	{
		NSLog(@"ldap_first_entry(): %s", ldap_err2string(err));
		ldap_msgfree(res);
		ldap_unbind_ext_s(ld, NULL, NULL);
		return nil;
	};
	
	while(entry)
	{
		//NSLog(@" ");
		//NSLog(@"dn: %s", ldap_get_dn(ld, entry));
		
		attribute = ldap_first_attribute(ld, entry, &ber);
		while(attribute)
		{
			if ((vals = ldap_get_values_len(ld, entry, attribute)))
			{
				//for(i = 0; vals[i]; i++)
				//	NSLog(@"%s: %s", attribute, vals[i]->bv_val);
			
				NSString* sattr1 = [[NSString stringWithCString:attribute] lowercaseString];
				
				char* szAttribute1 = [sattr1 cString];
				
				char* szAttribute = strtok(szAttribute1, ";");
				
				if(strcmp(szAttribute, attributes) == 0)
				{
					retData = [NSData dataWithBytes:vals[0]->bv_val length:vals[0]->bv_len];
					ldap_value_free_len(vals);
					ldap_memfree(attribute);
					ldap_msgfree(res);
					ber_free(ber, 0);
					ldap_unbind_ext_s(ld, NULL, NULL);
					
					return retData;					
				};
				
				ldap_value_free_len(vals);
				ldap_memfree(attribute);
			}
			attribute = ldap_next_attribute(ld, entry, ber);
		};
		
		ber_free(ber, 0);
		
		// skip to the next entry
		entry = ldap_next_entry(ld, entry);
	};
	
	//NSLog(@" ");
	
	//NSLog(@"unbinding from LDAP server...");
	ldap_msgfree(res);
	ldap_unbind_ext_s(ld, NULL, NULL);

	return retData;
}
*/
#endif
