#include "Precompiled.h"
#include "Ms2DObject.h"
#pragma message("©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥Î´Íê³É©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥©¥")

CMs2DObject::CMs2DObject(VOID)
: m_fPosX           (INVALID_NID)
, m_fPosY           (INVALID_NID)
, m_fWidth          (INVALID_NID)
, m_fLength         (INVALID_NID)
, m_fWalkSpeed      (0)
, m_fRunSpeed       (0)








, m_dwInPoolIndex   (INVALID_UID)
{
    ZeroMemoryArray(m_szName);
    ZeroMemoryStruct(m_ftLandTime);
}

CMs2DObject::~CMs2DObject(VOID)
{

}

CONST TCHAR* CMs2DObject::GetName(VOID)
{
    return m_szName;
}

VOID CMs2DObject::SetName(CONST TCHAR* szName, DWORD dwNameLen)
{
    if (dwNameLen > _2DE_MAX_OBJ_NAME_LEN)
    {
        _tcscpy_s(m_szName, _2DE_MAX_OBJ_NAME_LEN, szName);
    }
    else
    {
        _tcscpy_s(m_szName, dwNameLen, szName);
    }
}

DWORD CMs2DObject::GetNameLen(VOID)
{
    return (DWORD)_tcslen(m_szName);
}

VOID CMs2DObject::GetPos(FPOS& fPosX, FPOS& fPosY)
{
    fPosX = m_fPosX;
    fPosY = m_fPosY;
}

VOID CMs2DObject::SetPos(FPOS fPosX, FPOS fPosY)
{
    m_fPosX = fPosX;
    m_fPosY = fPosY;
}

VOID CMs2DObject::GetSize(FSIZE& fWidth, FSIZE& fLength)
{
    fWidth  = m_fWidth;
    fLength = m_fLength;
}

VOID CMs2DObject::SetSize(FSIZE fWidth, FSIZE fLength)
{
    if (fWidth > _2DE_MAX_OBJ_SIZE)
    {
        fWidth = _2DE_MAX_OBJ_SIZE;
    }

    if (fLength > _2DE_MAX_OBJ_SIZE)
    {
        fLength = _2DE_MAX_OBJ_SIZE;
    }

    m_fWidth  = fWidth;
    m_fLength = fLength;
}

FLOAT CMs2DObject::GetRotary(VOID)
{
    return m_fRotary;
}

VOID CMs2DObject::SetRotary(FLOAT fRotary)
{
    m_fRotary = fRotary;
}

FLOAT CMs2DObject::GetWalkSpeed(VOID)
{
    return m_fWalkSpeed;
}

VOID CMs2DObject::SetWalkSpeed(FLOAT fWalkSpeed)
{
    if (fWalkSpeed > _2DE_MAX_OBJ_MOVE_SPEED)
    {
        fWalkSpeed = _2DE_MAX_OBJ_MOVE_SPEED;
    }
    m_fWalkSpeed = fWalkSpeed;
}

FLOAT CMs2DObject::GetRunSpeed(VOID)
{
    return m_fRunSpeed;
}

VOID CMs2DObject::SetRunSpeed(FLOAT fRunSpeed)
{
    if (m_fRunSpeed > _2DE_MAX_OBJ_MOVE_SPEED)
    {
        m_fRunSpeed = _2DE_MAX_OBJ_MOVE_SPEED;
    }
    m_fRunSpeed = fRunSpeed;
}