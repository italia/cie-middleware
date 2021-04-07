#include "CIESign.h"
#include "disigonsdk.h"
#include <string.h>


CIESign::CIESign(IAS *ias){
	this->ias = ias;

}



uint16_t CIESign::sign(const char* inFilePath, const char* type, const char* pin, int page, float x, float y, float w, float h, const char* imagePathFile,const char* outFilePath)
{

	uint16_t response;

	DISIGON_CTX ctx = NULL;
	long ret = 0;

	try
	{
		ctx = disigon_sign_init();

#if 1
		ret = disigon_set(DISIGON_OPT_LOG_LEVEL, (void*)LOG_TYPE_DEBUG);
		if (ret != 0)
		{
			throw ret;
		}
#endif
		ret = disigon_sign_set(ctx, DISIGON_OPT_IAS_INSTANCE, (IAS*)(this->ias));
		if (ret != 0)
		{
			throw ret;
		}
		ret = disigon_sign_set(ctx, DISIGON_OPT_CADES, (void*)1);
		if (ret != 0)
		{
			throw ret;
		}
		ret = disigon_sign_set(ctx, DISIGON_OPT_PIN, (void*)pin);
		if (ret != 0)
		{
			throw ret;
		}

		ret = disigon_sign_set(ctx, DISIGON_OPT_INPUTFILE, (void*)inFilePath);
		if (ret != 0)
		{
			throw ret;
		}

		ret = disigon_sign_set(ctx, DISIGON_OPT_OUTPUTFILE, (void*)outFilePath);
		if (ret != 0)
		{
			throw ret;
		}

		if (strcmp(type, "pdf") == 0)
		{
			ret = disigon_sign_set(ctx, DISIGON_OPT_PDF_SUBFILTER, (void*)DISIGON_PDF_SUBFILTER_ETSI_CADES);
			if (ret != 0)
			{
				throw ret;
			}

			ret = disigon_sign_set(ctx, DISIGON_OPT_PDF_PAGE, (void*)&page);
			if (ret != 0)
			{
				throw ret;
			}

			ret = disigon_sign_set(ctx, DISIGON_OPT_PDF_LEFT, (void*)&x);
			if (ret != 0)
			{
				throw ret;
			}

			ret = disigon_sign_set(ctx, DISIGON_OPT_PDF_BOTTOM, (void*)&y);
			if (ret != 0)
			{
				throw ret;
			}

			ret = disigon_sign_set(ctx, DISIGON_OPT_PDF_WIDTH, (void*)&w);
			if (ret != 0)
			{
				throw ret;
			}

			ret = disigon_sign_set(ctx, DISIGON_OPT_PDF_HEIGHT, (void*)&h);
			if (ret != 0)
			{
				throw ret;
			}

			if (imagePathFile)
			{
				ret = disigon_sign_set(ctx, DISIGON_OPT_PDF_IMAGEPATH, (void*)imagePathFile);
				if (ret != 0)
				{
					throw ret;
				}
			}

			ret = disigon_sign_set(ctx, DISIGON_OPT_INPUTFILE_TYPE, (void*)DISIGON_FILETYPE_PDF);
			if (ret != 0)
			{
				throw ret;
			}
		}
		else {

			//TODO renderlo case insensitive
			if ((strstr(inFilePath, "p7m") != NULL) || (strstr(inFilePath, "p7s") != NULL))
				ret = disigon_sign_set(ctx, DISIGON_OPT_INPUTFILE_TYPE, (void*)DISIGON_FILETYPE_P7M);
			else
				ret = disigon_sign_set(ctx, DISIGON_OPT_INPUTFILE_TYPE, (void*)DISIGON_FILETYPE_PLAINTEXT);

			if (ret != 0)
			{
				throw ret;
			}

			ret = disigon_sign_set(ctx, DISIGON_OPT_DETACHED, (void*)0);
			if (ret != 0)
			{
				throw ret;
			}
		}

		ret = disigon_sign_set(ctx, DISIGON_OPT_VERIFY_USER_CERTIFICATE, 0);
		if (ret != 0)
		{
			throw ret;
		}


		ret = disigon_sign_sign(ctx);
		if (ret != 0)
		{
			throw ret;
		}
	}
	catch (long err) {
		Log.write("CIESign::sign error %d", err);;	
	}
	catch (...)
	{
		return 1000;
	}

	if (ctx)
		disigon_sign_cleanup(ctx);

	response = ret;

	return response;

}