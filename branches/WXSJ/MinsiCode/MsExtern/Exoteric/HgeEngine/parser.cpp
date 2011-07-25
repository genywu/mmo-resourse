#include "Precompiled.h"
/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Resource script parser implementation
*/

#include "parser.h"


HGE *RScriptParser::hge=0;


struct keyword
{
    TCHAR*    word;
    INT        code;
};

keyword keytable[]=
{
    { _T("="),            TTEQUALS        },
    { _T(":"),            TTBASED            },
    { _T("),"),            TTSEPARATOR        },
    { _T("{"),            TTOPENBLOCK        },
    { _T("}"),            TTCLOSEBLOCK    },
    { _T("TRUE"),        TTBOOL            },
    { _T("FALSE"),        TTBOOL            },

    { _T("Include"),    TTRES_INCLUDE    },
    { _T("Resource"),    TTRES_RESOURCE    },
    { _T("Texture"),    TTRES_TEXTURE    },
    { _T("Sound"),        TTRES_SOUND        },
    { _T("Music"),        TTRES_MUSIC        },
    { _T("Stream"),        TTRES_STREAM    },
    { _T("Target"),        TTRES_TARGET    },
    { _T("Sprite"),        TTRES_SPRITE    },
    { _T("Animation"),    TTRES_ANIMATION    },
    { _T("Font"),        TTRES_FONT        },
    { _T("Particle"),    TTRES_PARTICLE    },
    { _T("Distortion"),    TTRES_DISTORT    },
    { _T("StringTable"),TTRES_STRTABLE    },

    { _T("filename"),    TTPAR_FILENAME    },
    { _T("resgroup"),    TTPAR_RESGROUP    },
    { _T("mipmap"),        TTPAR_MIPMAP    },
    { _T("amplify"),    TTPAR_AMPLIFY    },
    { _T("size"),        TTPAR_SIZE        },
    { _T("zbuffer"),    TTPAR_ZBUFFER    },
    { _T("texture"),    TTPAR_TEXTURE    },
    { _T("rect"),        TTPAR_RECT        },
    { _T("hotspot"),    TTPAR_HOTSPOT    },
    { _T("blendmode"),    TTPAR_BLENDMODE    },
    { _T("color"),        TTPAR_COLOR        },
    { _T("zorder"),        TTPAR_ZORDER    },
    { _T("flip"),        TTPAR_FLIP        },
    { _T("scale"),        TTPAR_SCALE        },
    { _T("proportion"),    TTPAR_PROPORTION},
    { _T("rotation"),    TTPAR_ROTATION    },
    { _T("frames"),        TTPAR_FRAMES    },
    { _T("fps"),        TTPAR_FPS        },
    { _T("mode"),        TTPAR_MODE        },
    { _T("tracking"),    TTPAR_TRACKING    },
    { _T("spacing"),    TTPAR_SPACING    },
    { _T("sprite"),        TTPAR_SPRITE    },
    { _T("mesh"),        TTPAR_MESH        },

    { _T("COLORMUL"),    TTCON_COLORMUL    },
    { _T("COLORADD"),    TTCON_COLORADD    },
    { _T("ALPHABLEND"),    TTCON_ALPHABLND    },
    { _T("ALPHAADD"),    TTCON_ALPHAADD    },
    { _T("ZWRITE"),        TTCON_ZWRITE    },
    { _T("NOZWRITE"),    TTCON_NOZWRITE    },
    { _T("FORWARD"),    TTCON_FORWARD    },
    { _T("REVERSE"),    TTCON_REVERSE    },
    { _T("PINGPONG"),    TTCON_PINGPONG    },
    { _T("NOPINGPONG"),    TTCON_NOPINGPONG},
    { _T("LOOP"),        TTCON_LOOP        },
    { _T("NOLOOP"),        TTCON_NOLOOP    },
    { _T("CIRCLE"),        TTCON_CIRCLE    },
    { _T("RECT"),        TTCON_RECT        },
    { _T("ALPHA"),        TTCON_ALPHA        },

    { NULL,            TTNONE            }
};

RScriptParser::RScriptParser(TCHAR *name, TCHAR *scr)
{
    hge=hgeCreate(HGE_VERSION);

    scriptname=name;
    script=scr;
    tokenvalue[0]=0;
    tokentype=TTNONE;
    line=1;
}

INT RScriptParser::get_token()
{
    INT i;

    // Skip whitespaces and comments

    for(;;)
    {
        while(*script==' ' || *script=='\t' || *script=='\n' || *script=='\r')
        {
            if(*script=='\n') line++;
            script++;
        }
        if(*script==';') while(*script && *script!='\n' && *script!='\r') script++;
        else break;
    }

    // End of script

    if(!*script) { tokentype=TTEND; tokenvalue[0]=0; return tokentype; }

    // Number

    if((*script>='0' && *script<='9') || *script=='.' || *script=='-')
    {
        tokentype=TTNUMBER;
        for(i=0;(*script>='0' && *script<='9') || *script=='.' || *script=='-';i++)
             tokenvalue[i]=*script++;

        // Hexadecimal number starting with decimal digit

        if((*script>='A' && *script<='F') || (*script>='a' && *script<='f'))
        {
            tokentype=TTSTRING;
            for(; (*script>='A' && *script<='F') || (*script>='a' && *script<='f') ; i++)
                 tokenvalue[i]=*script++;
        }

        tokenvalue[i]=0;
        return tokentype;
    }

    // Quoted string

    if(*script=='"')
    {
        tokentype=TTSTRING;
        script++;
        for(i=0;*script && *script!='"' && *script!='\n' && *script!='\r';i++)
             tokenvalue[i]=*script++;
        tokenvalue[i]=0;
        if(*script) script++;
        return tokentype;
    }

    // Keyword

    for(i=0;keytable[i].word;i++)
        if(!strtkcmp(keytable[i].word, script))
        {
            tokentype = keytable[i].code;
            _tcscpy(tokenvalue,keytable[i].word);
            script+=_tcslen(keytable[i].word);
            return tokentype;
        }

    // Unquoted string or hexadecimal number

    tokentype=TTSTRING;
    for(i=0;
        *script && *script!=' ' && *script!='\t' && *script!='\n' && *script!='\r'
        && *script!=',' && *script!='=' && *script!='{' && *script!='}' && *script!=':';
        i++)
        tokenvalue[i]=*script++;
    tokenvalue[i]=0;
    return tokentype;
}

BOOL RScriptParser::strtkcmp(TCHAR *str, TCHAR *mem)
{
    INT i,len=(INT)_tcslen(str);
    for(i=0;i<len;i++)
    {
        if(!mem[i]) return TRUE;
        if(mem[i] != str[i]) return TRUE;
    }
    return FALSE;
}

DWORD RScriptParser::tkn_hex()
{
    INT i;
    DWORD dw=0;
    TCHAR chr;
    for(i=0; tokenvalue[i]; i++)
    {
        chr=tokenvalue[i];
        if(chr >= 'a') chr-='a'-':';
        if(chr >= 'A') chr-='A'-':';
        chr-='0';
        if(chr>0xF) chr=0xF;
        dw=(dw << 4) | chr;
    }
    return dw;
}

VOID RScriptParser::ScriptPostError(TCHAR *msg1, TCHAR *msg2)
{
    hge->System_Log(_T("%s, line %d: %s'%s'%s"),
        get_name(), get_line(), msg1, tokenvalue[0] ? tkn_string():_T("<EOF>"), msg2);
}
