// MorgenGrauen MUDlib
//
// living/attributes.h -- living attributes
//
// $Id: attributes.h 6282 2007-05-09 20:55:17Z Zesstra $
 
#ifndef __LIVING_ATTRIBUTES_H__
#define __LIVING_ATTRIBUTES_H__

// properties

#define P_ATTRIBUTES              "attributes"
#define P_ATTRIBUTES_OFFSETS      "attributes_offsets"
#define P_ATTRIBUTES_MODIFIER     "attributes_modifier"
#define P_X_ATTR_MOD              "extern_attributes_modifier"
#define P_X_HEALTH_MOD            "extern_health_modifier"
#define P_M_ATTR_MOD              "magic_attributes_modifier"
#define P_M_HEALTH_MOD            "magic_health_modifier"
#define P_ABILITIES               "abilities"
#define P_TIMED_ATTR_MOD		  "timed_attr_mod"

// special defines

/* standard attributes */
#define A_STR "str"
#define A_INT "int"
#define A_DEX "dex"
#define A_CON "con"

/* errorcodes for P_TIMED_ATTR_MOD */
#define TATTR_OK				0
#define TATTR_NO_SUCH_MODIFIER	1
#define TATTR_INVALID_ARGS  	2

/* indices used for P_TIMED_ATTR_MOD */
#define TATTR_OUTDATE		0
#define TATTR_DEPENDENTS	1
#define TATTR_ENTRIES		2	
#define TATTR_MOD 		0
#define TATTR_OUTDATED 		1
#define TATTR_DEPENDENT 	2
#define TATTR_NOTIFY 		3

/* maximum of cumulative attribute modifiers */
#define CUMULATIVE_ATTR_LIMIT	4

#endif // __LIVING_ATTRIBUTES_H__

#ifdef NEED_PROTOTYPES

#ifndef __LIVING_ATTRIBUTES_H_PROTO__
#define __LIVING_ATTRIBUTES_H_PROTO__

// prototypes

nomask public int SetTimedAttrModifier(string key, mapping modifier, int outdated, object dependent, mixed notify);  
nomask public mapping QueryTimedAttrModifier(string key);
nomask public int DeleteTimedAttrModifier(string key);

public int SetAttr(string attr, int val);
nomask public void register_modifier(object modifier);
nomask public void deregister_modifier(object modifier);

public int SetAttribute(string attr, int val);
public int SetRealAttribute(string attr, int val);
public int QueryAttribute(string attr);
public int QueryRealAttribute(string attr);
public int QueryAttributeOffset(string attr);

public void UpdateAttributes();

public status TestLimitViolation(mapping check);

// Set- und Query-Methoden
static mapping _set_attributes(mapping arr);
static mapping _query_attributes();
static mapping _set_attributes_offsets(mapping arr);
static mapping _query_attributes_offsets();
static mixed   _set_attributes_modifier(mixed arr);
static mapping _query_attributes_modifier();
static mixed _query_timed_attr_mod();

// internal
nomask protected void attribute_hb();
protected void add_offsets(mapping arr);
protected nomask void calculate_valid_modifiers();
protected nomask void notifyInvalidModifiers();
protected int _filterattr_str(int val);
protected int _filterattr_dex(int val);
protected int _filterattr_int(int val);
protected int _filterattr_con(int val);
nomask protected void notifyExpiredModifiers(mapping nots);

#endif // __LIVING_ATTRIBUTES_H_PROTO__

#endif // NEED_PROTOYPES
