// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"

TBitField::TBitField(int len)
{
    if (len < 0)
        throw length_error("negative length");

    BitLen = len;
    MemLen = (BitLen + ((1 << shift) - 1)) >> shift;
    if (MemLen) {
        pMem = new TELEM[MemLen];
        memset(pMem, 0, size_t(MemLen) << (shift - 3));
    }
    else {
        pMem = nullptr;
    }
}

TBitField::TBitField(const TBitField &bf) // конструктор копирования
{
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    if (MemLen) {
        pMem = new TELEM[MemLen];
        memcpy(pMem, bf.pMem, size_t(MemLen) << (shift - 3));
    }
    else {
        pMem = nullptr;
    }
}

TBitField::~TBitField()
{
    delete[] pMem;
}

int TBitField::GetMemIndex(const int n) const // индекс pМем для бита n
{
    if (n < 0 || n >= BitLen)
        throw invalid_argument("wrong index");
    return n >> shift;
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{
    if (n < 0 || n >= BitLen)
        throw invalid_argument("wrong index");
    return TELEM{ 1 } << (n - ((n >> shift) << shift));
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
    return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
    pMem[GetMemIndex(n)] |= GetMemMask(n);
}

void TBitField::ClrBit(const int n) // очистить бит
{
    pMem[GetMemIndex(n)] &= ~GetMemMask(n);
}

int TBitField::GetBit(const int n) const // получить значение бита
{
    return (pMem[GetMemIndex(n)] & GetMemMask(n)) != 0;
}

TBitField& TBitField::operator=(const TBitField &bf) // присваивание
{
    if (this == &bf)
        return *this;
    
    BitLen = bf.BitLen;
    if (MemLen != bf.MemLen) {
        delete[] pMem;
        MemLen = bf.MemLen;
        if (MemLen) {
            pMem = new TELEM[MemLen];
        }
        else {
            pMem = nullptr;
        }
    }
    if (MemLen) {
        memcpy(pMem, bf.pMem, size_t(MemLen) << (shift - 3));
    }
    return *this;
}

int TBitField::operator==(const TBitField &bf) const // сравнение
{
    int mLen = min(MemLen, bf.MemLen);
    int i = 0;
    while (i < mLen) {
        if (pMem[i] != bf.pMem[i])
            return 0;
        i++;
    }
    while (i < MemLen) {
        if (pMem[i])
            return 0;
        i++;
    }
    while (i < bf.MemLen) {
        if (bf.pMem[i])
            return 0;
        i++;
    }
    return 1;
}

int TBitField::operator!=(const TBitField &bf) const // сравнение
{
    return !(*this == bf);
}

// битовые операции

TBitField TBitField::operator|(const TBitField &bf) // операция "или"
{
    TBitField temp(max(BitLen, bf.BitLen));
    int i = 0;
    int mLen = min(MemLen, bf.MemLen);
    while (i < mLen) {
        temp.pMem[i] = pMem[i] | bf.pMem[i];
        i++;
    }
    while (i < MemLen) {
        temp.pMem[i] = pMem[i];
        i++;
    }
    while (i < bf.MemLen) {
        temp.pMem[i] = bf.pMem[i];
        i++;
    }
    return temp;
}

TBitField TBitField::operator&(const TBitField &bf) // операция "и"
{
    TBitField temp(max(BitLen, bf.BitLen));
    int mLen = min(MemLen, bf.MemLen);
    int i = 0;
    while (i < mLen) {
        temp.pMem[i] = pMem[i] & bf.pMem[i];
        i++;
    }
    return temp;
}

TBitField TBitField::operator~(void) // отрицание
{
    TBitField temp(*this);
    for (int i = 0; i < MemLen; i++) {
        temp.pMem[i] = ~pMem[i];
    }

    temp.pMem[MemLen - 1] &= ((GetMemMask(BitLen - 1) << 1) - 1);

    return temp;
}

// ввод/вывод

istream &operator>>(istream &istr, TBitField &bf) // ввод
{
    char temp;
    for (int i = 0; i < bf.BitLen; i++) {
        istr >> temp;
        if (temp == '1')
            bf.SetBit(i);
        else if (temp == '0')
            bf.ClrBit(i);
        else
            throw invalid_argument{"wrong input"};
    }
    return istr;
}

ostream &operator<<(ostream &ostr, const TBitField &bf) // вывод
{
    for (int i = 0; i < bf.BitLen; i++)
        ostr << bf.GetBit(i);
    return ostr;
}
