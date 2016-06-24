// MorgenGrauen MUDlib
//
// thing/material.h -- material handling header
//
// $Id: material.h 6930 2008-08-09 09:20:16Z Zesstra $

#ifndef __THING_MATERIAL_H__
#define __THING_MATERIAL_H__

#define P_MATERIAL              "material"
#define MATERIALDB              "/secure/materialdb"
#define P_MATERIAL_KNOWLEDGE    "material_knowledge"
#define MATERIAL_SYMMETRIC_RECOGNIZABILITY "mat_sym_recoc"

// Materialien nicht beim Laden durch MatDB includen
// TODO: besser recoc-propertydefinitionen woanders rein und Materialien hier
// immer includen
#ifndef _SKIP_MATERIALS_
#include <materials.h>
#endif

#ifdef NEED_PROTOTYPES
#ifndef __THING_MATERIAL_H_PROTO__
#define __THING_MATERIAL_H_PROTO__

int QueryMaterial(string mat);
int QueryMaterialGroup(string matgroup);
string MaterialList(int casus, mixed idinf);

#endif // __THING_MATERIAL_H_PROTO__
#endif // NEED_PROTOTYPES
#endif // __THING_MATERIAL_H__
