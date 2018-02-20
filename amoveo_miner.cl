#ifndef uint32_t
#define uint32_t unsigned int
#endif

#define H0 0x6a09e667
#define H1 0xbb67ae85
#define H2 0x3c6ef372
#define H3 0xa54ff53a
#define H4 0x510e527f
#define H5 0x9b05688c
#define H6 0x1f83d9ab
#define H7 0x5be0cd19


uint rotr(uint x, int n) {
  if (n < 32) return (x >> n) | (x << (32 - n));
  return x;
}

uint ch(uint x, uint y, uint z) {
  return (x & y) ^ (~x & z);
}

uint maj(uint x, uint y, uint z) {
  return (x & y) ^ (x & z) ^ (y & z);
}

uint sigma0(uint x) {
  return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

uint sigma1(uint x) {
  return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

uint gamma0(uint x) {
  return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

uint gamma1(uint x) {
  return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}


__kernel void amoveo_mine(__global uchar *Z) {
  //Z[32] = 20;

  //increment the nonce.
  uchar carry_flag = 1;
  for (uint i = 0; i < 32; i++) {
    if (carry_flag == 1) {
      if (Z[32 + i] == 255) {
	Z[32 + i] = 0;
      } else {
	Z[32 + i] ++;
	carry_flag = 0;
      }
    }
  }

  //preparing for sha256
  uint data_info[3];
  //char plain_key[66] = "a";

  uchar plain_key[67];// = "a";
  //uint kernel_id = get_global_id(0) % 2;
  //printf("kernel id %u\n", kernel_id);
  for (uint i = 0; i < 66; i++) {
    plain_key[i] = Z[i];
  }
  //plain_key[63] = kernel_id;
  
  data_info[0] = 64;
  data_info[1] = 1;//global work size
  data_info[2] = strlen(plain_key);
  //printf("%d\n", data_info[2]);
  uint digest[8];
  for (uint i = 0; i < 8; i++) {
    digest[i] = 0;
  }

  //SHA256 starts.

  //}
////# __kernel void sha256_crypt_kernel(__global uint *data_info,__global char *plain_key,  __global uint *digest){
//void hash(uint* data_info, uint* plain_key, uint* digest){
//void hash(){
  int t, gid, msg_pad;
  int stop, mmod;
  uint i, ulen, item, total;
  uint W[80], temp, A,B,C,D,E,F,G,H,T1,T2;
  uint num_keys = data_info[1];
  int current_pad;

  uint K[64]={
0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

  msg_pad=0;

  ulen = data_info[2];
  //total = ulen%64>=56?2:1 + ulen/64;
  total = 1;

//  printf("ulen: %u total:%u\n", ulen, total);

  digest[0] = H0;
  digest[1] = H1;
  digest[2] = H2;
  digest[3] = H3;
  digest[4] = H4;
  digest[5] = H5;
  digest[6] = H6;
  digest[7] = H7;
  for(item=0; item<total; item++)
  {

    A = digest[0];
    B = digest[1];
    C = digest[2];
    D = digest[3];
    E = digest[4];
    F = digest[5];
    G = digest[6];
    H = digest[7];

#pragma unroll
    for (t = 0; t < 80; t++){
    W[t] = 0x00000000;
    }
    msg_pad=item*64;
    if(ulen > msg_pad)
    {
      current_pad = (ulen-msg_pad)>64?64:(ulen-msg_pad);
    }
    else
    {
      current_pad =-1;    
    }

  //  printf("current_pad: %d\n",current_pad);
    if(current_pad>0)
    {
      i=current_pad;

      stop =  i/4;
  //    printf("i:%d, stop: %d msg_pad:%d\n",i,stop, msg_pad);
      for (t = 0 ; t < stop ; t++){
        W[t] = ((uchar)  plain_key[msg_pad + t * 4]) << 24;
        W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 1]) << 16;
        W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 2]) << 8;
        W[t] |= (uchar)  plain_key[msg_pad + t * 4 + 3];
        //printf("W[%u]: %u\n",t,W[t]);
      }
      mmod = i % 4;
      if ( mmod == 3){
        W[t] = ((uchar)  plain_key[msg_pad + t * 4]) << 24;
        W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 1]) << 16;
        W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 2]) << 8;
        W[t] |=  ((uchar) 0x80) ;
      } else if (mmod == 2) {
        W[t] = ((uchar)  plain_key[msg_pad + t * 4]) << 24;
        W[t] |= ((uchar) plain_key[msg_pad + t * 4 + 1]) << 16;
        W[t] |=  0x8000 ;
      } else if (mmod == 1) {
        W[t] = ((uchar)  plain_key[msg_pad + t * 4]) << 24;
        W[t] |=  0x800000 ;
	} else { //if (mmod == 0)
        W[t] =  0x80000000 ;
      }
      
      if (current_pad<56)
      {
        W[15] =  ulen*8 ;
        //printf("ulen avlue 2 :w[15] :%u\n", W[15]);
      }
    }
    else if(current_pad <0)
    {
      if( ulen%64==0)
        W[0]=0x80000000;
      W[15]=ulen*8;
      //printf("ulen avlue 3 :w[15] :%u\n", W[15]);
    }

    for (t = 0; t < 64; t++) {
      if (t >= 16)
        W[t] = gamma1(W[t - 2]) + W[t - 7] + gamma0(W[t - 15]) + W[t - 16];
      T1 = H + sigma1(E) + ch(E, F, G) + K[t] + W[t];
      T2 = sigma0(A) + maj(A, B, C);
      H = G; G = F; F = E; E = D + T1; D = C; C = B; B = A; A = T1 + T2;
    }
    digest[0] += A;
    digest[1] += B;
    digest[2] += C;
    digest[3] += D;
    digest[4] += E;
    digest[5] += F;
    digest[6] += G;
    digest[7] += H;

    //SHA256 ends.
    //hash2integer
    //(256*number of leading 0 bits) + byte starting with 1.
    uchar digest_bytes[32];
    for (uint i = 0; i < 8; i++) {
      digest_bytes[(i*4)+ 3] = digest[i] % 256;
      digest_bytes[(i*4)+ 2] = (digest[i] / 256) % 256;
      digest_bytes[(i*4)+ 1] = ((digest[i] / 256)/256) % 256;
      digest_bytes[(i*4)+ 0] = (((digest[i] / 256)/256)/256);
    }
    uint our_diff = 0;
    uchar diff_flag = 1;
    uchar hash_int;
    uchar hash_bit;
    uchar hash_bit2;
    //202 is 011001010 = 2+8+64+128
    for(uint i = 0; i < 256; i++) {
      if (diff_flag == 1) {
	hash_int = i / 8;
	hash_bit = (7 - (i % 8));
	hash_bit2 = ((digest_bytes[hash_int]) >> hash_bit) & 1;
	//printf("hashbit2 %d\n", hash_bit2);
	//printf("hashbit %d\n", hash_bit);
	//printf("digest_bytes[0] %d\n", digest_bytes[hash_int]);
	//printf("digest_bytes[0] %d\n", digest_bytes[hash_int+1]);
	//printf("digest_bytes[0] %d\n", digest_bytes[hash_int] << (i % 8));
	//printf("digest_bytes[0] %d\n", digest_bytes[hash_int+1] >> (hash_bit + 1));
	if (hash_bit2 == 1) {
	  diff_flag = 0;
	  our_diff *= 256;
	  our_diff += ((digest_bytes[hash_int] << (i % 8)) + (digest_bytes[hash_int+1] >> (hash_bit + 1)));
	} else {
	  our_diff++;
	}
      }
    }
    //printf("our diff %u\n", our_diff);
    //check if our_diff > difficulty
    //if it is, then save the nonce.
    uint difficulty = (256 * Z[64]) + Z[65];
    if (our_diff > difficulty) {
      for (uint i = 0; i < 32; i++) {
	Z[68+i] = Z[32+i];
      }
    }



    //for testing purposes
    //for (uint i = 0; i < 32; i++) {
    //  Z[i] = digest_bytes[i];
    //}
  
    //  for (t = 0; t < 80; t++)
  //    {
  //    printf("W[%d]: %u\n",t,W[t]);
  //    }
  }
}


