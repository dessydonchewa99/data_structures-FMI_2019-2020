//
//  ArchFile.cpp
//  darch
//
//  Created by Desislava Doncheva on 24.12.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#if defined(WIN32) || defined(_WIN32)
#include <dirent_win.h> //use "dirent_win.h" for Windows
#else
#include <dirent.h>
#endif
#include "ArchFile.hpp"
#include "sha256.hpp"
#include "speck.hpp"

using namespace std;

//default constructor
ArchFile::ArchFile()
{
    char EmptyPass[MAX_PASSWORD];
    strcpy(EmptyPass, "");
    strcpy(archFileName, "");
    fileOpenedForRead = false;
    fileOpenedForWrite = false;
    setPassword(EmptyPass);
    fp = nullptr;
}

//check if password is set
bool ArchFile::hasPassword()
{
    int i;
    for(i=0; i<strlen(password); i++)
    {
        if(password[i] != PASS_ALIGN_CHAR)
        {
            return true;
        }
    }
    return false;
}

//check if filenname pass to add/create command is a file or file mask
void ArchFile::CheckFilesToAdd(char *path)
{
    char matchFile[MAX_FILE_NAME];
    strcpy(matchFile, path);
    char *findDot;
    char *findStar;
    
    findDot = strrchr(matchFile, '.');
    if(findDot != nullptr) // that is a file
    {
        findStar = strrchr(matchFile, '*');
        if(findStar != nullptr) // this is a file mask
        {
            enumerateFileMask(matchFile);
        }
        else // this is a single file
        {
             writeFileToArch(matchFile);
        }
    }
    else // that is a directory
    {
        if(matchFile[strlen(matchFile) - 1] != PATH_SEPARATOR_CHAR )
        {
            strcat(matchFile, PATH_SEPARATOR_STR);
        }
        strcat(matchFile, "*.*");
        enumerateFileMask(matchFile);
    }
}

//enumerates all files matching fileMask and scan all sub directories too
void ArchFile::enumerateFileMask(char *fileMask)
{
    char fileName[MAX_FILE_NAME];
    char dirStr[MAX_FILE_NAME], maskStr[MAX_FILE_NAME];
    char newFileName[MAX_FILE_NAME];
    char newSubDirNameAndMask[MAX_FILE_NAME];
    char *findDirSep;
    
    strcpy(fileName, fileMask);
    findDirSep = strrchr(fileName, PATH_SEPARATOR_CHAR);
    //split path to path and file mask
    if(findDirSep != nullptr)
    {
        *findDirSep = '\0';
        strcpy(dirStr, fileName);
        strcpy(maskStr, findDirSep+1);
    }
    else
    {
        strcpy(dirStr, ".");
        strcpy(maskStr, fileMask);
    }
    DIR *dir = opendir(dirStr);
    if(dir)
    {
        struct dirent *ent;
        while((ent = readdir(dir)) != NULL) //scan all files/dirs in that directory
        {
            if(strcmp(ent->d_name, ".") != 0 and strcmp(ent->d_name, "..") != 0) //skip these, not real directories
            {
                if(ent->d_type == DT_DIR) //dir
                {
                    strcpy(newSubDirNameAndMask, dirStr);
                    strcat(newSubDirNameAndMask, PATH_SEPARATOR_STR);
                    strcat(newSubDirNameAndMask, ent->d_name);
                    strcat(newSubDirNameAndMask, PATH_SEPARATOR_STR);
                    strcat(newSubDirNameAndMask, maskStr);
                    enumerateFileMask(newSubDirNameAndMask); //recursive call to sub dir
                }
                else //file
                {
                    strcpy(newFileName, dirStr);
                    strcat(newFileName, PATH_SEPARATOR_STR);
                    strcat(newFileName, ent->d_name);
                    //check mask
                    if(CheckFileMask(ent->d_name, maskStr))
                    {
                        //write that file to archive
                        writeFileToArch(newFileName);
                    }
                }
            }
        }
    }
    else
    {
        cout << "Error opening directory " << dirStr << endl;
    }
}

//compare file name and file mask for matching
bool ArchFile::CheckFileMask(char *Filename, char *Mask)
{
    bool retFlag = false;
    char fileExt[MAX_FILE_NAME];
    char maskExt[MAX_FILE_NAME];
    char *findDot;
    
    strcpy(fileExt, "");
    strcpy(maskExt, "");
    
    if(strcmp(Mask, "*.*") == 0 or strcmp(Mask, "*") == 0) //all files
    {
        retFlag = true;
    }
    else //specific *.ext
    {
        findDot = strrchr(Filename, '.');
        if(findDot)
        {
            strcpy(fileExt, findDot+1);
        }
        findDot = strrchr(Mask, '.');
        if(findDot)
        {
            strcpy(maskExt, findDot+1);
        }
        strToUpper(fileExt);
        strToUpper(maskExt);
        if(strcmp(fileExt, maskExt) == 0) //mask match
        {
            retFlag = true;
        }
    }
    return retFlag;
}

//writres a single sprecific file ot archive
void ArchFile::writeFileToArch(char* file)
{
    uint8_t inBytes[FILE_BLOCK_SIZE], outBytes[FILE_BLOCK_SIZE];
    int tailBytes;
    unsigned int i, j;
    FileStruct tempFileSt;
    FILE *inFile;
    inFile = fopen(file, "rb"); // open input file
    
    if(inFile!=nullptr)
    {
        tempFileSt.nameSize = strlen(file)+1;
        strcpy(tempFileSt.name, file);
        fseek(inFile, 0, SEEK_END);
        tempFileSt.fileSize = (uint32_t)ftell(inFile);
        fseek(inFile, 0, SEEK_SET);

        cout << "adding file " << file << " (" << tempFileSt.fileSize<< " bytes)" << endl;

        //write to archive file file header fields
        fwrite(&tempFileSt.nameSize, 1, 2, fp);
        fwrite(&tempFileSt.name, 1, tempFileSt.nameSize, fp);
        fwrite(&tempFileSt.fileSize, 1, 4, fp);
        //write bytes of the input file to output archive file
        for(i = 0; i < (tempFileSt.fileSize/FILE_BLOCK_SIZE); i++)
        {
            fread(&inBytes, 1, FILE_BLOCK_SIZE, inFile);
            Encrypt(inBytes, outBytes, FILE_BLOCK_SIZE, password); //encrypt bytes, if necessary
            fwrite(&outBytes, 1, FILE_BLOCK_SIZE, fp);
            
        }
        //write last block of the file
        tailBytes = tempFileSt.fileSize % FILE_BLOCK_SIZE;
        if(tailBytes != 0)
        {
            for(j=0; j<FILE_BLOCK_SIZE; j++)
            {
                inBytes[j] = 0;
            }
            fread(&inBytes, 1, tailBytes, inFile);
            Encrypt(inBytes, outBytes, FILE_BLOCK_SIZE, password); //encrypt bytes, if necessary
            fwrite(&outBytes, 1, FILE_BLOCK_SIZE, fp);
        }
        fclose(inFile);
    }
    else
    {
        cout << "Cannot open " << file << " for reading" << endl;
    }
}

//remove a single file from archive
size_t ArchFile::removeFileToArch(size_t remainingBytes, char *fileToRemove)
{
    size_t roundFileSize;
    size_t remBytes;
    remBytes = remainingBytes;
    size_t ThisFileSize = 0;
    size_t i;
    unsigned char onebyte;
    size_t DeletedBytes = 0;
    
    while(remBytes > 0) //scan whole file
    {
        FileStruct tempFStruct;
        //read file header
        fread(&tempFStruct.nameSize, 1, 2, fp);
        fread(&tempFStruct.name, 1, tempFStruct.nameSize, fp);
        fread(&tempFStruct.fileSize, 1, 4, fp);
        //skip file size bytes(rouded to FILE_BLOCK_SIZE bytes up)
        roundFileSize = RoundedFileSize(tempFStruct.fileSize);
        ThisFileSize = (2 + tempFStruct.nameSize + 4 + roundFileSize);
        //file name match
        if(strcmp(tempFStruct.name, fileToRemove) == 0) //remove this file
        {
            cout << "Remove file " << tempFStruct.name << endl;
            fseek(fp, -(4 + (int32_t)tempFStruct.nameSize + 2), SEEK_CUR);
            //copy remaining bytes of the archive file, skiping this file
            for(i=0; i<(remainingBytes - ThisFileSize); i++)
            {
                fseek(fp, ThisFileSize, SEEK_CUR);
                fread(&onebyte, 1, 1, fp);
                fseek(fp, -((int32_t)ThisFileSize+1), SEEK_CUR);
                fwrite(&onebyte, 1, 1, fp);
            }
            DeletedBytes = ThisFileSize;
            break;
        }
        else
        {
            fseek(fp, roundFileSize, SEEK_CUR);
        }
        
        remBytes = remBytes - (2 + tempFStruct.nameSize + 4 + roundFileSize);
    }
    return DeletedBytes;
}


//prints all files in Archive file
void ArchFile::ListFilesInArch (size_t remainingBytes)
{
    size_t roundFileSize;
    size_t remBytes;
    remBytes = remainingBytes;
    
    while(remBytes > 0) //scan all files
    {
        FileStruct tempFStruct;
        //read file header
        fread(&tempFStruct.nameSize, 1, 2, fp);
        fread(&tempFStruct.name, 1, tempFStruct.nameSize, fp);
        fread(&tempFStruct.fileSize, 1, 4, fp);
        //skip file size bytes(rouded to FILE_BLOCK_SIZE bytes up)
        roundFileSize = RoundedFileSize(tempFStruct.fileSize);
        //print file info
        cout << "File: " << tempFStruct.name << ", size: " << tempFStruct.fileSize << endl;
        fseek(fp, roundFileSize, SEEK_CUR);
        
        remBytes = remBytes - (2 + tempFStruct.nameSize + 4 + roundFileSize);
    }
}

//extract all files from Archive file to target path
void ArchFile::ExtractArchFile(size_t remainingBytes, char *pathToExtr)
{
    size_t roundFileSize;
    size_t remBytes;
    uint8_t inBytes[FILE_BLOCK_SIZE], outBytes[FILE_BLOCK_SIZE];
    char ExtractFileName[MAX_FILE_NAME];
    FILE *Extrfp;
    uint32_t i;
    int tailBytesCnt;
    
    remBytes = remainingBytes;
    while(remBytes > 0) //scan all files
    {
        //read file header
        FileStruct tempFStruct;
        fread(&tempFStruct.nameSize, 1, 2, fp);
        fread(&tempFStruct.name, 1, tempFStruct.nameSize, fp);
        fread(&tempFStruct.fileSize, 1, 4, fp);
        //file size bytes(rouded to FILE_BLOCK_SIZE bytes up)
        roundFileSize = RoundedFileSize(tempFStruct.fileSize);
        //---extract file
        strcpy(ExtractFileName, pathToExtr);
        strcat(ExtractFileName, PATH_SEPARATOR_STR);
        strcat(ExtractFileName, tempFStruct.name);
        tryCreatePath(ExtractFileName);
        Extrfp = fopen(ExtractFileName, "w+b");
        if(Extrfp != nullptr)
        {
            cout << "Extracting file " << ExtractFileName << endl;
            //read file in blocks
            for(i=0; i<tempFStruct.fileSize/FILE_BLOCK_SIZE; i++)
            {
                fread(inBytes, 1, FILE_BLOCK_SIZE, fp);
                Decrypt(inBytes, outBytes, FILE_BLOCK_SIZE, password); //decrypt it first, if necessary
                fwrite(outBytes, 1, FILE_BLOCK_SIZE, Extrfp);
            }
            //read last block
            tailBytesCnt = tempFStruct.fileSize % FILE_BLOCK_SIZE;
            if(tailBytesCnt > 0)
            {
                fread(inBytes, 1, FILE_BLOCK_SIZE, fp);
                Decrypt(inBytes, outBytes, FILE_BLOCK_SIZE, password); //decrypt it first, if necessary
                fwrite(outBytes, 1, tailBytesCnt, Extrfp);
            }
            fclose(Extrfp);
        }
        else
        {
            cout << "Error writing to file " << ExtractFileName << endl;
            fseek(fp, roundFileSize, SEEK_CUR);
        }
        
        //---
        remBytes = remBytes - (2 + tempFStruct.nameSize + 4 + roundFileSize);
    }
}
//encrypt input bytes
void ArchFile::Encrypt(uint8_t *inBlock, uint8_t *outBlock, int BlockSize, char* password)
{
    int i;
    uint64_t in[2];
    uint64_t out[2];
    uint64_t key[2];
   
    if(inBlock == nullptr or outBlock == nullptr)
    {
        return;
    }
    if(hasPassword() and BlockSize == 16 and strlen(password) >= 16)
    {
        if(BlockSize == 16)
        {
            in[0] = 0x0000000000000000;
            in[1] = 0x0000000000000000;
            for(i=0; i<8; i++)
            {
                in[0] = in[0] | (((uint64_t)inBlock[i]) << ((7-i)*8));
                in[1] = in[1] | (((uint64_t)inBlock[8+i]) << ((7-i)*8));
            }
            key[0] = 0x0000000000000000;
            key[1] = 0x0000000000000000;
            for(i=0; i<8; i++)
            {
                key[0] = key[0] | (((uint64_t)password[i]) << ((7-i)*8));
                key[1] = key[1] | (((uint64_t)password[8+i]) << ((7-i)*8));
            }
            encrypt(out, in, key);
            for(i=0; i<8; i++)
            {
                outBlock[i]   = (int8_t)((out[0] >> ((7-i)*8)) & 0x00000000000000FF);
                outBlock[8+i] = (int8_t)((out[1] >> ((7-i)*8)) & 0x00000000000000FF);
            }
        }
    }
    else // no password, return same bytes
    {
        for(i=0; i<BlockSize; i++)
        {
            outBlock[i] = inBlock[i];
        }
    }
}

//Decrypt input bytes
void ArchFile::Decrypt(uint8_t *inBlock, uint8_t *outBlock, int BlockSize, char* password)
{
    int i;
    uint64_t in[2];
    uint64_t out[2];
    uint64_t key[2];
    
    if(inBlock == nullptr or outBlock == nullptr)
    {
        return;
    }
    if(hasPassword() and BlockSize == 16 and strlen(password) >= 16)
    {
        if(BlockSize == 16)
        {
            in[0] = 0x0000000000000000;
            in[1] = 0x0000000000000000;
            for(i=0; i<8; i++)
            {
                in[0] = in[0] | (((uint64_t)inBlock[i]) << ((7-i)*8));
                in[1] = in[1] | (((uint64_t)inBlock[8+i]) << ((7-i)*8));
            }
            key[0] = 0x0000000000000000;
            key[1] = 0x0000000000000000;
            for(i=0; i<8; i++)
            {
                key[0] = key[0] | (((uint64_t)password[i]) << ((7-i)*8));
                key[1] = key[1] | (((uint64_t)password[8+i]) << ((7-i)*8));
            }

            decrypt(out, in, key);
            for(i=0; i<8; i++)
            {
                outBlock[i]   = (int8_t)((out[0] >> ((7-i)*8)) & 0x00000000000000FF);
                outBlock[8+i] = (int8_t)((out[1] >> ((7-i)*8)) & 0x00000000000000FF);
            }
        }
    }
    else // no password, return same bytes
    {
        for(i=0; i<BlockSize; i++)
        {
            outBlock[i] = inBlock[i];
        }

    }
}

//create path(dirs and subdirs) to that file
void ArchFile::tryCreatePath(char *pathToFile)
{
    char pathToCreate[MAX_FILE_NAME];
    char *lastDirSepPtr = nullptr;
    char makeDirCmd[MAX_FILE_NAME];
    strcpy(pathToCreate, pathToFile);
    
    //check for last dir separator sign to get anly path
    lastDirSepPtr = strrchr(pathToCreate, PATH_SEPARATOR_CHAR);
    if(lastDirSepPtr != nullptr)
    {
        *lastDirSepPtr = '\0';
        if(strlen(pathToCreate) > 0)
        {
            strcpy(makeDirCmd, MKDIR_CMD);
            strcat(makeDirCmd, " ");
            strcat(makeDirCmd, pathToCreate);
            system(makeDirCmd); //call 'mkdir' command
        }
    }
}
//open Archive file for read/write and create it, if necessary
bool ArchFile::Open(char* archFileName, bool createIfNotExist, bool openForRead, bool openForWrite)
{
    char openMode[10];
    strcpy(openMode, "");
    
    Close();
    strcpy(this->archFileName, archFileName);
    
    if(createIfNotExist)
    {
        CreateArchFileIfNotExist(archFileName);
    }
    if(openForRead == true and openForWrite == false)
    {
        strcpy(openMode, "r+b");
        fileOpenedForRead = true;
        fileOpenedForWrite = false;
    }
    if(openForRead == false and openForWrite == true)
    {
        strcpy(openMode, "w+b");
        fileOpenedForRead = false;
        fileOpenedForWrite = true;
    }
    if(openForRead == true and openForWrite == true)
    {
        strcpy(openMode, RWFILEMODE);
        fileOpenedForRead = true;
        fileOpenedForWrite = true;
    }
    if(openForRead == false and openForWrite == false)
    {
        fileOpenedForRead = false;
        fileOpenedForWrite = false;
        return false;
    }
    fp = fopen(archFileName, openMode);
    if(fp == nullptr)
    {
        fileOpenedForRead = false;
        fileOpenedForWrite = false;
        return false;
    }
    return true;
}

//check of Archive file is opened
bool ArchFile::isOpened()
{
    if(fp != nullptr)
    {
        return true;
    }
    return false;
}

//get archive file size
size_t ArchFile::getFileSize()
{
    size_t fileLen = 0;
    if(isOpened())
    {
        if(fseek(fp, 0, SEEK_END) == 0)
        {
            fileLen = ftell(fp);
            fseek(fp, 0, SEEK_SET);
        }
    }
    return fileLen;
}

//close archive file
void ArchFile::Close()
{
    if(fp!=nullptr)
    {
        fclose(fp);
        fp = nullptr;
    }
    fileOpenedForRead = false;
    fileOpenedForWrite = false;
}

//check if Archive file already exists
bool ArchFile::CheckArchExist(char* archFileName)
{
    FILE *fptemp;
    fptemp = fopen(archFileName, "rb");
    if(fptemp != nullptr)
    {
        fclose(fptemp);
        return true;
    }
    return false;
}

//Create empty Archive file
bool ArchFile::CreateArchFileIfNotExist(char* archFileName)
{
    if(CheckArchExist(archFileName)!=true)
    {
        FILE *fptemp;
        fptemp = fopen(archFileName, "wb");
        if(fptemp != nullptr)
        {
            fclose(fptemp);
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

//set password for archive
void ArchFile::setPassword(char* pass)
{
    int i;
    char extPass[MAX_PASSWORD];
    int passLen;

    strncpy(extPass, pass, FILE_BLOCK_SIZE);
    passLen = (int)strlen(extPass);
    if(passLen < FILE_BLOCK_SIZE)
    {
        for(i=0; i<(FILE_BLOCK_SIZE-passLen); i++)
        {
            strcat(extPass, PASS_ALIGN_STR);
        }
    }
    strcpy(password, extPass);
    archHeader.passHash.calcHash(password);
}

// ------------------------
//create archive command
void ArchFile::ArchCreate (char *filesToAdd)
{
    unsigned long ArchFileSize = 0;
    
    if(isOpened() == true and fileOpenedForWrite == true) //check if it is opend
    {
        ArchFileSize = getFileSize();
        if(ArchFileSize == 0) //new Arch file
        {
            cout << "Adding files to new " << archFileName << " archive:" << endl;
            fseek(fp, 0, SEEK_SET);
            fwrite(&archHeader.sign, 1, ARCH_PREFIX_LEN, fp);
            fwrite(&archHeader.passHash, 1, HASH_LEN, fp);
            CheckFilesToAdd(filesToAdd);
        }
        else // existing Arch file, adding files to it
        {
            if(ArchFileSize >= (ARCH_PREFIX_LEN + HASH_LEN))
            {
                //check for valid archive file prefix
                fseek(fp, 0, SEEK_SET);
                HeaderStruct readHeader;
                fread(&readHeader.sign, 1, ARCH_PREFIX_LEN, fp);
                fread(&readHeader.passHash, 1, HASH_LEN, fp);
                if(readHeader.sign != ARCH_PREFIX)
                {
                    cout << "File " << archFileName << " is not correct archive file" << endl;
                    return;
                }
                //check password
                if(archHeader.passHash.compare(readHeader.passHash) == false)
                {
                    cout << "Password is incorrect" << endl;
                    return;
                }
                fseek(fp, 0, SEEK_END);
                cout << "Adding files to existing " << archFileName << " archive:" << endl;
                CheckFilesToAdd(filesToAdd);
            }
            else
            {
                cout << "File " << archFileName << " is not correct archive file" << endl;
            }
        }
    }
    else
    {
        cout << "File " << archFileName << " cannot be opened for writing" << endl;
    }
}

void ArchFile::ArchExtract(char *pathToExtract)
{
    unsigned long ArchFileSize = 0;
    
    if(isOpened() == true and fileOpenedForRead == true) //check if it is opend
    {
        ArchFileSize = getFileSize();
        
        if(ArchFileSize >= (ARCH_PREFIX_LEN + HASH_LEN))
        {
            //check for valid archive file prefix
            fseek(fp, 0, SEEK_SET);
            HeaderStruct readHeader;
            fread(&readHeader.sign, 1, ARCH_PREFIX_LEN, fp);
            fread(&readHeader.passHash, 1, HASH_LEN, fp);
            if(readHeader.sign != ARCH_PREFIX)
            {
                cout << "File " << archFileName << " is not correct archive file" << endl;
                return;
            }
            //check password
            if(archHeader.passHash.compare(readHeader.passHash) == false)
            {
                cout << "Password is incorrect" << endl;
                return;
            }
            cout << "Extract " << archFileName << " archive to " << pathToExtract << endl;
            ExtractArchFile(ArchFileSize - (ARCH_PREFIX_LEN + HASH_LEN), pathToExtract);
        }
        else
        {
            cout << "File " << archFileName << " is not correct archive file" << endl;
        }
    }
    else
    {
        cout << "File " << archFileName << " cannot be opened for reading" << endl;
    }
}

void ArchFile::ArchList()
{
    unsigned long ArchFileSize = 0;
    
    if(isOpened() == true and fileOpenedForRead == true) //check if it is opend
    {
        ArchFileSize = getFileSize();
       
        if(ArchFileSize >= (ARCH_PREFIX_LEN + HASH_LEN))
        {
            //check for valid archive file prefix
            fseek(fp, 0, SEEK_SET);
            HeaderStruct readHeader;
            fread(&readHeader.sign, 1, ARCH_PREFIX_LEN, fp);
            fread(&readHeader.passHash, 1, HASH_LEN, fp);
            if(readHeader.sign != ARCH_PREFIX)
            {
                cout << "File " << archFileName << " is not correct archive file" << endl;
                return;
            }
            //check password
            if(archHeader.passHash.compare(readHeader.passHash) == false)
            {
                cout << "Password is incorrect" << endl;
                return;
            }
            cout << "List all files in " << archFileName << " archive:" << endl;
            ListFilesInArch(ArchFileSize - (ARCH_PREFIX_LEN + HASH_LEN));
        }
        else
        {
            cout << "File " << archFileName << " is not correct archive file" << endl;
        }
    }
    else
    {
        cout << "File " << archFileName << " cannot be opened for reading" << endl;
    }
}

void ArchFile::ArchAdd(char *filesToAdd)
{
    unsigned long ArchFileSize = 0;
    
    if(isOpened() == true and fileOpenedForWrite == true) //check if it is opend
    {
        ArchFileSize = getFileSize();
        if(ArchFileSize >= (ARCH_PREFIX_LEN + HASH_LEN))
        {
            //check for valid archive file prefix
            fseek(fp, 0, SEEK_SET);
            HeaderStruct readHeader;
            fread(&readHeader.sign, 1, ARCH_PREFIX_LEN, fp);
            fread(&readHeader.passHash, 1, HASH_LEN, fp);
            if(readHeader.sign != ARCH_PREFIX)
            {
                cout << "File " << archFileName << " is not correct archive file" << endl;
                return;
            }
            //check password
            if(archHeader.passHash.compare(readHeader.passHash) == false)
            {
                cout << "Password is incorrect" << endl;
                return;
            }
            fseek(fp, 0, SEEK_END);
            cout << "Adding files to existing " << archFileName << " archive:" << endl;
            CheckFilesToAdd(filesToAdd);
        }
        else
        {
            cout << "File " << archFileName << " is not correct archive file" << endl;
        }
    }
    else
    {
        cout << "File " << archFileName << " cannot be opened for writing" << endl;
    }
}

void ArchFile::ArchRemove(char *fileToRemove)
{
    unsigned long ArchFileSize = 0;
    char tempArchFile[MAX_FILE_NAME];
    char *findDirSep = nullptr;
    FILE *inFile, *outFile;
    unsigned char oneByte;
    bool TransferOk = false;
    size_t i;
    if(isOpened() == true and fileOpenedForWrite == true) //check if it is opend
    {
        ArchFileSize = getFileSize();
        if(ArchFileSize >= (ARCH_PREFIX_LEN + HASH_LEN))
        {
            //check for valid archive file prefix
            fseek(fp, 0, SEEK_SET);
            HeaderStruct readHeader;
            fread(&readHeader.sign, 1, ARCH_PREFIX_LEN, fp);
            fread(&readHeader.passHash, 1, HASH_LEN, fp);
            if(readHeader.sign != ARCH_PREFIX)
            {
                cout << "File " << archFileName << " is not correct archive file" << endl;
                return;
            }
            //check password
            if(archHeader.passHash.compare(readHeader.passHash) == false)
            {
                cout << "Password is incorrect" << endl;
                return;
            }
            cout << "Remove file from " << archFileName << " archive:" << endl;
            size_t DeletedBytes;
            DeletedBytes = removeFileToArch(ArchFileSize - (ARCH_PREFIX_LEN + HASH_LEN), fileToRemove);
            if(DeletedBytes > 0) //copy to new file, rename to original name
            {
                Close();
                //archFileName
                strcpy(tempArchFile, archFileName);
                findDirSep = strrchr(tempArchFile, PATH_SEPARATOR_CHAR);
                if(findDirSep != nullptr)
                {
                    *findDirSep = '\0';
                    strcat(tempArchFile, PATH_SEPARATOR_STR);
                    strcat(tempArchFile, TEMP_ARCH_FILE);
                }
                else
                {
                    strcpy(tempArchFile, ".");
                    strcat(tempArchFile, PATH_SEPARATOR_STR);
                    strcat(tempArchFile, TEMP_ARCH_FILE);
                }
                inFile = fopen(archFileName, "rb");
                if(inFile != nullptr)
                {
                    outFile = fopen(tempArchFile, "w+b");
                    if(outFile != nullptr)
                    {
                        for(i=0; i<ArchFileSize-DeletedBytes; i++)
                        {
                            fread(&oneByte, 1, 1, inFile);
                            fwrite(&oneByte, 1, 1, outFile);
                        }
                        TransferOk = true;
                        fclose(outFile);
                    }
                    else
                    {
                        cout << "Cannot open file " << tempArchFile << " for writing" << endl;
                    }
                    fclose(inFile);
                }
                else
                {
                    cout << "Cannot open file " << archFileName << " for reading" << endl;
                }
                if(TransferOk)
                {
                    remove(archFileName); //remove old file
                    rename(tempArchFile, archFileName); //rename temp file to original name
                }
            }
        }
        else
        {
            cout << "File " << archFileName << " is not correct archive file" << endl;
        }
    }
    else
    {
        cout << "File " << archFileName << " cannot be opened for writing" << endl;
    }

}
//-----------------------------
ArchFile::~ArchFile()
{
    Close();
}
