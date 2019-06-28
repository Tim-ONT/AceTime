// This file was generated by the following script:
//
//  $ ../../../tools/tzcompiler.py --input_dir /home/brian/dev/tz --output_dir /home/brian/dev/AceTime/tests/ExtendedValidationUsingJavaTest/zonedbx2018g --tz_version 2018g --action zonedb --language arduino --scope extended --db_namespace zonedbx2018g --start_year 2000 --until_year 2050
//
// using the TZ Database files
//
//  africa, antarctica, asia, australasia, backward, etcetera, europe, northamerica, southamerica
//
// from https://github.com/eggert/tz/releases/tag/2018g
//
// DO NOT EDIT

#ifndef ACE_TIME_ZONEDBX2018G_ZONE_POLICIES_H
#define ACE_TIME_ZONEDBX2018G_ZONE_POLICIES_H

#include <ace_time/common/ZonePolicy.h>

namespace ace_time {
namespace zonedbx2018g {

//---------------------------------------------------------------------------
// Supported zone policies: 83
//
extern const extended::ZonePolicy kPolicyAN;
extern const extended::ZonePolicy kPolicyAQ;
extern const extended::ZonePolicy kPolicyAS;
extern const extended::ZonePolicy kPolicyAT;
extern const extended::ZonePolicy kPolicyAV;
extern const extended::ZonePolicy kPolicyAW;
extern const extended::ZonePolicy kPolicyArg;
extern const extended::ZonePolicy kPolicyArmenia;
extern const extended::ZonePolicy kPolicyAus;
extern const extended::ZonePolicy kPolicyAzer;
extern const extended::ZonePolicy kPolicyBarb;
extern const extended::ZonePolicy kPolicyBelize;
extern const extended::ZonePolicy kPolicyBrazil;
extern const extended::ZonePolicy kPolicyC_Eur;
extern const extended::ZonePolicy kPolicyCO;
extern const extended::ZonePolicy kPolicyCR;
extern const extended::ZonePolicy kPolicyCanada;
extern const extended::ZonePolicy kPolicyChatham;
extern const extended::ZonePolicy kPolicyChile;
extern const extended::ZonePolicy kPolicyCook;
extern const extended::ZonePolicy kPolicyCuba;
extern const extended::ZonePolicy kPolicyDhaka;
extern const extended::ZonePolicy kPolicyE_EurAsia;
extern const extended::ZonePolicy kPolicyEU;
extern const extended::ZonePolicy kPolicyEUAsia;
extern const extended::ZonePolicy kPolicyEcuador;
extern const extended::ZonePolicy kPolicyEgypt;
extern const extended::ZonePolicy kPolicyEire;
extern const extended::ZonePolicy kPolicyFalk;
extern const extended::ZonePolicy kPolicyFiji;
extern const extended::ZonePolicy kPolicyGhana;
extern const extended::ZonePolicy kPolicyGuat;
extern const extended::ZonePolicy kPolicyHK;
extern const extended::ZonePolicy kPolicyHaiti;
extern const extended::ZonePolicy kPolicyHoliday;
extern const extended::ZonePolicy kPolicyHond;
extern const extended::ZonePolicy kPolicyIran;
extern const extended::ZonePolicy kPolicyIraq;
extern const extended::ZonePolicy kPolicyJapan;
extern const extended::ZonePolicy kPolicyJordan;
extern const extended::ZonePolicy kPolicyKyrgyz;
extern const extended::ZonePolicy kPolicyLH;
extern const extended::ZonePolicy kPolicyLebanon;
extern const extended::ZonePolicy kPolicyLibya;
extern const extended::ZonePolicy kPolicyMacau;
extern const extended::ZonePolicy kPolicyMauritius;
extern const extended::ZonePolicy kPolicyMexico;
extern const extended::ZonePolicy kPolicyMoldova;
extern const extended::ZonePolicy kPolicyMoncton;
extern const extended::ZonePolicy kPolicyMongol;
extern const extended::ZonePolicy kPolicyMorocco;
extern const extended::ZonePolicy kPolicyNC;
extern const extended::ZonePolicy kPolicyNT_YK;
extern const extended::ZonePolicy kPolicyNZ;
extern const extended::ZonePolicy kPolicyNamibia;
extern const extended::ZonePolicy kPolicyNic;
extern const extended::ZonePolicy kPolicyPRC;
extern const extended::ZonePolicy kPolicyPakistan;
extern const extended::ZonePolicy kPolicyPalestine;
extern const extended::ZonePolicy kPolicyPara;
extern const extended::ZonePolicy kPolicyPeru;
extern const extended::ZonePolicy kPolicyPhil;
extern const extended::ZonePolicy kPolicyROK;
extern const extended::ZonePolicy kPolicyRussia;
extern const extended::ZonePolicy kPolicyRussiaAsia;
extern const extended::ZonePolicy kPolicySA;
extern const extended::ZonePolicy kPolicySalv;
extern const extended::ZonePolicy kPolicySanLuis;
extern const extended::ZonePolicy kPolicyStJohns;
extern const extended::ZonePolicy kPolicySudan;
extern const extended::ZonePolicy kPolicySyria;
extern const extended::ZonePolicy kPolicyTaiwan;
extern const extended::ZonePolicy kPolicyThule;
extern const extended::ZonePolicy kPolicyTonga;
extern const extended::ZonePolicy kPolicyTroll;
extern const extended::ZonePolicy kPolicyTunisia;
extern const extended::ZonePolicy kPolicyTurkey;
extern const extended::ZonePolicy kPolicyUS;
extern const extended::ZonePolicy kPolicyUruguay;
extern const extended::ZonePolicy kPolicyVanuatu;
extern const extended::ZonePolicy kPolicyWS;
extern const extended::ZonePolicy kPolicyWinn;
extern const extended::ZonePolicy kPolicyZion;


//---------------------------------------------------------------------------
// Unsupported zone policies: 55
//
// kPolicyAlbania (unused)
// kPolicyAlgeria (unused)
// kPolicyAustria (unused)
// kPolicyBahamas (unused)
// kPolicyBelgium (unused)
// kPolicyBulg (unused)
// kPolicyCA (unused)
// kPolicyChicago (unused)
// kPolicyCyprus (unused)
// kPolicyCzech (unused)
// kPolicyDR (unused)
// kPolicyDenmark (unused)
// kPolicyDenver (unused)
// kPolicyDetroit (unused)
// kPolicyE-Eur (unused)
// kPolicyEdm (unused)
// kPolicyEgyptAsia (unused)
// kPolicyFinland (unused)
// kPolicyFrance (unused)
// kPolicyGB-Eire (unused)
// kPolicyGermany (unused)
// kPolicyGreece (unused)
// kPolicyHalifax (unused)
// kPolicyHungary (unused)
// kPolicyIceland (unused)
// kPolicyIndianapolis (unused)
// kPolicyItaly (unused)
// kPolicyLatvia (unused)
// kPolicyLouisville (unused)
// kPolicyLux (unused)
// kPolicyMalta (unused)
// kPolicyMarengo (unused)
// kPolicyMenominee (unused)
// kPolicyNBorneo (unused)
// kPolicyNYC (unused)
// kPolicyNeth (unused)
// kPolicyNorway (unused)
// kPolicyPerry (unused)
// kPolicyPike (unused)
// kPolicyPoland (unused)
// kPolicyPort (unused)
// kPolicyPulaski (unused)
// kPolicyRegina (unused)
// kPolicyRomania (unused)
// kPolicyShang (unused)
// kPolicySovietZone (unused)
// kPolicySpain (unused)
// kPolicySpainAfrica (unused)
// kPolicyStarke (unused)
// kPolicySwift (unused)
// kPolicySwiss (unused)
// kPolicyToronto (unused)
// kPolicyVanc (unused)
// kPolicyVincennes (unused)
// kPolicyW-Eur (unused)


// Inaccurate zone policies: 3
//
// kPolicyMoncton (AT time '0:01' truncated to '900' seconds)
// kPolicyPalestine (AT time '0:01' truncated to '900' seconds)
// kPolicyStJohns (AT time '0:01' truncated to '900' seconds)


}
}

#endif