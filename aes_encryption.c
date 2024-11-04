/** 
*	@file aes_encryption.c
*	@brief AES-128 encryption algorithm implementation.
*
*	@author xuân việt
*/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "aes_encryption.h"


uint8_t s(uint8_t c){
	/* the table sbox below is qualified as static. because if it were not qualified as static, then every time this
	 * function is called, sbox array will be created on the function call Stack and then all the values are copied
	 * from Flash to the stack, which is very slow.
	 * sbox is also qualified as const, because it must not be changed, and also to make sure the values reside in Flash
	 * and not get copied to RAM. (maybe another qualifier is required such as _flash to make sure it stays in Flash
	 * it depends on the compiler)
	 */
	static const uint8_t sbox[] = {0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76,0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0,0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15,0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75,0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84,0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF,0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8,0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2,0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73,0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB,0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79,0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08,0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A,0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E,0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF,0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16};
	return sbox[c];
}


void shift_rows(uint8_t block[16]){
	uint8_t tmp0,tmp1;
	/* First row will be ignored, Second row will be shifted 1 element to the left */
	tmp0=block[1];block[1]=block[5];block[5]=block[9];block[9]=block[13];block[13]=tmp0;

	/*3rd row will be shifted 2 elements to the left */
	tmp0=block[2];tmp1=block[6];block[2]=block[10];block[6]=block[14];block[10]=tmp0;block[14]=tmp1;

	/*4th row will be shifted 3 elements to the left which is equivalent to 1 element shift to the right */
	tmp0=block[15];block[15]=block[11];block[11]=block[7];block[7]=block[3];block[3]=tmp0;
}

/**
 * @brief g is a transformation used in the calculation of the key schedule
 * @param uint32_t is the input of the g transform
 * @param uint8_t round_nbd is the AES-128 round number. 1-10
 * @return the transform of a
 */
uint32_t g(uint32_t a, uint8_t round_nbr){

	/*round coefficient RC is a lookup table*/
	static const uint8_t RC[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1B,0x36};
	/* v divides the input word a into 4 bytes */
	uint8_t v[4];


	/* b holds the return value*/
	uint32_t b;

	/* bytes from input a are inserted in vector v so that v looks like a but with
	 * a circular left shift.
	 */
	v[0] = (uint8_t)(a >> 16);
	v[1] = (uint8_t)(a>>8);
	v[2] = (uint8_t)a;
	v[3] = (uint8_t)(a>>24);

	/*apply substitution with s function to v*/
	v[0] = s(v[0]);
	v[1] = s(v[1]);
	v[2] = s(v[2]);
	v[3] = s(v[3]);

	/*add round number to v[0], addition is the XOR operator */
	v[0]=v[0]^RC[round_nbr];

	/* insert values of v in b in same order*/
	b=(uint32_t)v[3];
	b=b|(((uint32_t)v[2])<<8);
	b=b|(((uint32_t)v[1])<<16);
	b=b|(((uint32_t)v[0])<<24);

	return b;

}

void expand_key(uint8_t key[16],uint8_t round_nbr){
	/*v divides the input key into 4 words of 32 bits*/
	uint32_t v[4];

	/*w holds the output words*/
	uint32_t w[4];

	/** calculate and insert words in v. Every word represent 4 bytes from the key.
	 *  every 4 bytes in the key are mapped to a 32 bit word in v
	 */

	v[0]=( ((uint32_t)key[0])<<24 ) | ( ((uint32_t)key[1])<<16 ) | ( ((uint32_t)key[2])<<8 ) | ( ((uint32_t)key[3]) );
	v[1]=( ((uint32_t)key[4])<<24 ) | ( ((uint32_t)key[5])<<16 ) | ( ((uint32_t)key[6])<<8 ) | ( ((uint32_t)key[7]) );
	v[2]=( ((uint32_t)key[8])<<24 ) | ( ((uint32_t)key[9])<<16 ) | ( ((uint32_t)key[10])<<8 ) | ( ((uint32_t)key[11]) );
	v[3]=( ((uint32_t)key[12])<<24 ) | ( ((uint32_t)key[13])<<16 ) | ( ((uint32_t)key[14])<<8 ) | ( ((uint32_t)key[15]) );


	/*calculate the words that make up the next key*/

	w[0]=v[0]^g(v[3],round_nbr);
	w[1]=w[0]^v[1];
	w[2]=w[1]^v[2];
	w[3]=w[2]^v[3];

	/*move the bits from w to  key*/
	key[0]=(uint8_t)(w[0]>>24); key[1]=(uint8_t)(w[0]>>16);key[2]=(uint8_t)(w[0]>>8);key[3]=(uint8_t)w[0];
	key[4]=(uint8_t)(w[1]>>24); key[5]=(uint8_t)(w[1]>>16);key[6]=(uint8_t)(w[1]>>8);key[7]=(uint8_t)w[1];
	key[8]=(uint8_t)(w[2]>>24); key[9]=(uint8_t)(w[2]>>16);key[10]=(uint8_t)(w[2]>>8);key[11]=(uint8_t)w[2];
	key[12]=(uint8_t)(w[3]>>24); key[13]=(uint8_t)(w[3]>>16);key[14]=(uint8_t)(w[3]>>8);key[15]=(uint8_t)w[3];
}


uint8_t _mult(uint8_t a, uint8_t b){

	/* This lookup table is used to retrieve the multiplication of an element by 0x02 or by 0x03 in GF(2^8)
	 * The column number represents the
	 * multx02 represents the multiplication by 0X02
	 * multx03 represents the multiplication by 0X03
	 * In the mix_column transformation, only multiplication by 0X02 and 0X03 is performed.
	 * The tables are static so that they get initialized only one time and not every time the function is called.
	 */
	static const uint8_t multx02[]={0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e,0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,0x70,0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e,0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e,0x90,0x92,0x94,0x96,0x98,0x9a,0x9c,0x9e,0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae,0xb0,0xb2,0xb4,0xb6,0xb8,0xba,0xbc,0xbe,0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,0xde,0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xee,0xf0,0xf2,0xf4,0xf6,0xf8,0xfa,0xfc,0xfe,0x1b,0x19,0x1f,0x1d,0x13,0x11,0x17,0x15,0x0b,0x09,0x0f,0x0d,0x03,0x01,0x07,0x05,0x3b,0x39,0x3f,0x3d,0x33,0x31,0x37,0x35,0x2b,0x29,0x2f,0x2d,0x23,0x21,0x27,0x25,0x5b,0x59,0x5f,0x5d,0x53,0x51,0x57,0x55,0x4b,0x49,0x4f,0x4d,0x43,0x41,0x47,0x45,0x7b,0x79,0x7f,0x7d,0x73,0x71,0x77,0x75,0x6b,0x69,0x6f,0x6d,0x63,0x61,0x67,0x65,0x9b,0x99,0x9f,0x9d,0x93,0x91,0x97,0x95,0x8b,0x89,0x8f,0x8d,0x83,0x81,0x87,0x85,0xbb,0xb9,0xbf,0xbd,0xb3,0xb1,0xb7,0xb5,0xab,0xa9,0xaf,0xad,0xa3,0xa1,0xa7,0xa5,0xdb,0xd9,0xdf,0xdd,0xd3,0xd1,0xd7,0xd5,0xcb,0xc9,0xcf,0xcd,0xc3,0xc1,0xc7,0xc5,0xfb,0xf9,0xff,0xfd,0xf3,0xf1,0xf7,0xf5,0xeb,0xe9,0xef,0xed,0xe3,0xe1,0xe7,0xe5,};
	static const uint8_t multx03[]={0x00,0x03,0x06,0x05,0x0c,0x0f,0x0a,0x09,0x18,0x1b,0x1e,0x1d,0x14,0x17,0x12,0x11,0x30,0x33,0x36,0x35,0x3c,0x3f,0x3a,0x39,0x28,0x2b,0x2e,0x2d,0x24,0x27,0x22,0x21,0x60,0x63,0x66,0x65,0x6c,0x6f,0x6a,0x69,0x78,0x7b,0x7e,0x7d,0x74,0x77,0x72,0x71,0x50,0x53,0x56,0x55,0x5c,0x5f,0x5a,0x59,0x48,0x4b,0x4e,0x4d,0x44,0x47,0x42,0x41,0xc0,0xc3,0xc6,0xc5,0xcc,0xcf,0xca,0xc9,0xd8,0xdb,0xde,0xdd,0xd4,0xd7,0xd2,0xd1,0xf0,0xf3,0xf6,0xf5,0xfc,0xff,0xfa,0xf9,0xe8,0xeb,0xee,0xed,0xe4,0xe7,0xe2,0xe1,0xa0,0xa3,0xa6,0xa5,0xac,0xaf,0xaa,0xa9,0xb8,0xbb,0xbe,0xbd,0xb4,0xb7,0xb2,0xb1,0x90,0x93,0x96,0x95,0x9c,0x9f,0x9a,0x99,0x88,0x8b,0x8e,0x8d,0x84,0x87,0x82,0x81,0x9b,0x98,0x9d,0x9e,0x97,0x94,0x91,0x92,0x83,0x80,0x85,0x86,0x8f,0x8c,0x89,0x8a,0xab,0xa8,0xad,0xae,0xa7,0xa4,0xa1,0xa2,0xb3,0xb0,0xb5,0xb6,0xbf,0xbc,0xb9,0xba,0xfb,0xf8,0xfd,0xfe,0xf7,0xf4,0xf1,0xf2,0xe3,0xe0,0xe5,0xe6,0xef,0xec,0xe9,0xea,0xcb,0xc8,0xcd,0xce,0xc7,0xc4,0xc1,0xc2,0xd3,0xd0,0xd5,0xd6,0xdf,0xdc,0xd9,0xda,0x5b,0x58,0x5d,0x5e,0x57,0x54,0x51,0x52,0x43,0x40,0x45,0x46,0x4f,0x4c,0x49,0x4a,0x6b,0x68,0x6d,0x6e,0x67,0x64,0x61,0x62,0x73,0x70,0x75,0x76,0x7f,0x7c,0x79,0x7a,0x3b,0x38,0x3d,0x3e,0x37,0x34,0x31,0x32,0x23,0x20,0x25,0x26,0x2f,0x2c,0x29,0x2a,0x0b,0x08,0x0d,0x0e,0x07,0x04,0x01,0x02,0x13,0x10,0x15,0x16,0x1f,0x1c,0x19,0x1a};

	switch (b){
	case 0x01 : return a;break;
	case 0x02 : return multx02[a];break;
	case 0x03 : return multx03[a];break;
	default   : return 0x00;
	}
}

void mix_columns(uint8_t B[16]){
	uint8_t C[16];
	uint8_t m,k,l,temp=0;
	static const uint8_t M[]={0x02, 0x03, 0x01, 0x01, 0x01, 0x02, 0x03, 0x01, 0x01, 0x01, 0x02, 0x03, 0x03, 0x01, 0x01, 0x02};

	/* Calculate and fill C with the transformed values of B*/
	for(l=0;l<4;l++)
		for(k=0;k<4;k++){
			for(m=0;m<4;m++)
				temp^=_mult(B[4*k+m],M[4*l+m]);
			C[4*k+l]=temp;
			temp=0;
		}
	memcpy(B,C,16);
}

void aes128_encrypt(uint8_t txt[16], uint8_t key[16]){
	/*
	 * Note:
	 * The plain text is shuffled 10 times, called rounds.
	 * the values of the plain text in each round is called state matrix.
	 *
	 * Pseudo code for aes128:
	 * Begin AES
	 *
	 * XOR plain text with the key
	 * Repeat 10 times
	 *	substitute bytes using sbox
	 * 	shift Rows
	 * 	Mix Columns (in the last round, mix columns is ommited)
	 * 	Derive next key in the schedule
	 * 	XOR state matrix with the key schedule
	 *
	 * 	End AES
	 */

	uint8_t i,j;

	/*XOR plain text with the key*/
	for(i=0;i<16;i++)
		txt[i]=txt[i]^key[i];
	for(j=0;j<10;j++){
		/*substitute bytes using sbox*/
		for(i=0;i<16;i++)
				txt[i]=s(txt[i]);
		shift_rows(txt);
		if (j!=9)
			mix_columns(txt);
		expand_key(key,j);
		for(i=0;i<16;i++)
				txt[i]=txt[i]^key[i];

	}
}

