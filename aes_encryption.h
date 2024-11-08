/** 
*	@file aes_encryption.h
*	@brief AES-128 functions' prototypes and definition of related parameter
*	@author xuân việt
*/

#ifndef SRC_AES_H_
#define SRC_AES_H_
#include <stdint.h>

/**
 * @brief encrypts a block of 16 bytes (128 bits)
 * @param uint8_t txt[16] is the input block to be encrypted
 * @param uint8_t key[16] is the encryption key.
 */
void aes128_encrypt(uint8_t txt[16], uint8_t key[16]);

/**
 * @brief s is a transformation that maps an 8-bit input, c, to an 8-bit output according to the S-Box.
 * the S-Box is a lookup table that represents the The Rijndael substitution box.
 * @param c is the input
 * @return s returns the transformation value
 */
uint8_t s(uint8_t c);

/**
 * @brief expand_key generates the next key in the key schedule
 * @param uint8_t previous_key[16] is the starting key
 * @param uint8_t next_key[16] is the newly generated key from the previous_key
 * @param uint8_t round is the round number in the AES algorithm. values = 1-10
 *
 */
void expand_key(uint8_t previous_key[16],uint8_t round);


/**
 * @brief does a circular left shift on the rows of the input 4x4 matrix.
 * 	1st row is not shifted, 2nd row is shifted 1 element, 3d row is shifted 2 elements, 4th row is shifted 3 elements.
 * @param uint8_t block[16] The 16 bytes length input is regarded as a 4x4 matrix. The rows of this matrix will be shifted as described in the brief section.
 */
void shift_rows(uint8_t block[16]);

/**
 * @brief  performs a linear transformation on every column in the 4x4 input matrix. Every column is multiplied
 * by the matrix below:
 * 	0x02, 0x03, 0x01, 0x01
 * 	0x01, 0x02, 0x03, 0x01
 * 	0x01, 0x01, 0x02, 0x03
 * 	0x03, 0x01, 0x01, 0x02
 * 	The multiplication is an operation in GF(2^8) which means it is not a simple multiplication of natural numbers.
 * 	since there is only multiplication by 0x01, 0x02, and 0x03, a lookup table containing all the results of multiplying the values
 * 	[0-256] by 0x02 and 0x03 is pre-calculated and initialized.. Multiplication by 0x01 results in the same value.
 * @param uint8_t block[16] The 16 bytes length input is regarded as a 4x4 matrix.
 */
void mix_columns(uint8_t block[16]);

/**
 * @brief does the multiplication of 2 elements in Galois Field GF(2^8).
 * it can do only multiplication of any element in 0-255 by 1,2 and 3.
 * @param uint8_t a a value in 0-255
 * @param uint8_t b a value in {1,2,3}
 * @return the multiplication result, which is a value in 0-256
 */
uint8_t _mult(uint8_t a, uint8_t b);



#endif /* SRC_AES_H_ */
