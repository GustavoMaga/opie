
/*
 * This header file defines some structures and types normally found in the
 * Palm SDK.  However, I don't want to require the presense of the SDK for a
 * small utility since most Palm owners won't have it.
 *
 * $Id: pdb.h,v 1.8 2005-05-05 14:39:34 pohly Exp $
 *
 */

#ifndef __PDB_H__
#define __PDB_H__

#ifndef _WINDOWS
#include <netinet/in.h>
#endif
#include <stdio.h>

#include "CExpander.h"

/* Normal Palm typedefs */
#include "mytypes.h"
typedef UInt32          LocalID;

/* Max length of DB name */
#define dmDBNameLength 0x20


/************************************************************
 * Structure of a Record entry
 *************************************************************/
typedef struct {
    LocalID localChunkID;         // local chunkID of a record
    UInt8   attributes;           // record attributes;
    UInt8   uniqueID[3];          // unique ID of record; should
                                // not be 0 for a legal record.
} RecordEntryType;


/************************************************************
 * Structure of a record list extension. This is used if all
 *  the database record/resource entries of a database can't fit into
 *  the database header.
 *************************************************************/
typedef struct {
    LocalID nextRecordListID;     // local chunkID of next list
    UInt16  numRecords;           // number of records in this list
    UInt16  firstEntry;           // array of Record/Rsrc entries
                                // starts here
} RecordListType;


/************************************************************
 * Structure of a Database Header
 *************************************************************/
typedef struct {
    UInt8   name[dmDBNameLength]; // name of database
    UInt16  attributes;           // database attributes
    UInt16  version;              // version of database
    UInt32  creationDate;         // creation date of database
    UInt32  modificationDate;     // latest modification date
    UInt32  lastBackupDate;       // latest backup date
    UInt32  modificationNumber;   // modification number of database
    LocalID appInfoID;            // application specific info
    LocalID sortInfoID;           // app specific sorting info
    UInt32  type;                 // database type
    UInt32  creator;              // database creator
    UInt32  uniqueIDSeed;         // used to generate unique IDs.
                                //   Note that only the low order
                                //   3 bytes of this is used (in
                                //   RecordEntryType.uniqueID).
                                //   We are keeping 4 bytes for
                                //   alignment purposes.
    RecordListType recordList;    // first record list
} DatabaseHdrType;


class Cpdb : public CExpander
{
 protected:
    size_t file_length;
    FILE* fin;
    size_t recordpos(int);
    size_t recordlength(int);
    void gotorecordnumber(int);
    DatabaseHdrType head;
    bool openpdbfile(const char* src);
    Cpdb() : fin(NULL) {}
    ~Cpdb();
#ifdef USEQPE
    void suspend();
    void unsuspend();
#else
    void suspend() {}
    void unsuspend() {}
#endif
 public:
    virtual void sizes(unsigned long& _file, unsigned long& _text) = 0;
};
#endif

