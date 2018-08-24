#include <stdio.h>
#include <gcrypt.h>

#include "baseencode.h"
#include "otp.h"
#include "totp.h"

#define PERIOD 60
#define SECRET_LENGTH 32
#define SHA1 GCRY_MD_SHA1
#define SHA256 GCRY_MD_SHA256
#define SHA512 GCRY_MD_SHA512

totps_t* generate_totp() {
	int i;
	FILE *fp;
	baseencode_error_t base_err;
	cotp_error_t err;
	totps_t* totps = malloc(sizeof(totps_t));
	char email_secret[SECRET_LENGTH+1] = {0};
	char phone_secret[SECRET_LENGTH+1] = {0};


	if ((fp = fopen("/dev/urandom", "r")) == NULL) {
		fprintf(stderr, "[TOTP.C]: Error! Could not open /dev/urandom for read\n");
		return NULL;
	}
	for(i = 0; i < SECRET_LENGTH; i++){
		do {
			email_secret[i] = fgetc(fp);
		} while (email_secret[i] == '\0');
	}
	for(i = 0; i < SECRET_LENGTH; i++){
		do {
			phone_secret[i] = fgetc(fp);
		} while (phone_secret[i] == '\0');
	}

	totps->access_code_email = base32_encode(email_secret, SECRET_LENGTH+1, &base_err);
	totps->access_code_phone = base32_encode(phone_secret, SECRET_LENGTH+1, &base_err);
	totps->email_totp = get_totp(totps->access_code_email, EMAIL_TOTP_LENGTH, PERIOD, SHA256, &err);
	totps->phone_totp = get_totp(totps->access_code_phone, PHONE_TOTP_LENGTH, PERIOD, SHA256, &err);

	return totps;
}

char* validate_totp(char* key, char* totp, int length) {
	return totp_verify(key, totp, length, PERIOD, SHA256);
}

void free_totps(totps_t* totps) {
	if (totps != NULL) {
		if (totps->access_code_email != NULL)
			free(totps->access_code_email);
		if (totps->access_code_phone != NULL)
			free(totps->access_code_phone);
		if (totps->phone_totp != NULL)
			free(totps->phone_totp);
		if (totps->email_totp != NULL)
			free(totps->email_totp);
		free(totps);
	}
}
