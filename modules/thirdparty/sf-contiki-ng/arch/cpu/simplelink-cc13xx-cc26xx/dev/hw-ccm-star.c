#ifdef __cplusplus
extern "C" {
#endif

/**
 * @code
 *  ___ _____ _   ___ _  _____ ___  ___  ___ ___
 * / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 * \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 * |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 * embedded.connectivity.solutions.==============
 * @endcode

 * @copyright  STACKFORCE GmbH, Germany, www.stackforce.de
 * @author     STACKFORCE
 * @brief      Hardware AES-128 CCM module.
 *
 * This file implements the hardware AES CCM calls.
 *
 */

#include <string.h>
/* Module interface. */
#include "hw-ccm-star.h"
/* Software AES-128 APIs. */
#include "lib/aes-128.h"
/* Hardware AES-128 CCM driver implementation APIs. */
#include <ti/drivers/AESCCM.h>
/* API to initialize and access plaintext CryptoKeys. */
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>

/* The auth data can have a variable length, but this implementation supports
 * only the case of 0 < l(a) < (2^16 - 2^8) */
#define MAX_A_LEN 0xfeff
/* Valid values are 4, 6, 8, 10, 12, 14, and 16 octets */
#define MIC_LEN_VALID(x) ((x) >= 4 && (x) <= 16 && (x) % 2 == 0)
/* Define name for AESCCM channel index. */
#define AESCCM_INSTANCE 0
/* Invalid MIC. */
#define MIC_INVALID (uint8_t[]) {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,\
                                 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}

/* Encryption key storage. */
static uint8_t encKey[AES_128_KEY_LENGTH] = {0};

/*---------------------------------------------------------------------------*/
static void
set_key(const uint8_t *key)
{
  if(NULL != key)
  {
    /* Store the key in the RAM. */
    memcpy(encKey, (uint8_t*)key, AES_128_KEY_LENGTH);
  }
}

/*---------------------------------------------------------------------------*/
static void
aead(const uint8_t* nonce, uint8_t* m, uint16_t m_len, const uint8_t* a,
     uint16_t a_len, uint8_t *result, uint8_t mic_len, int forward)
{
  AESCCM_Handle handle;
  AESCCM_Params params;
  CryptoKey cryptoKey;
  AESCCM_Operation operation;
  int_fast16_t encryptionResult;

  if((MAX_A_LEN < a_len) || (false == MIC_LEN_VALID(mic_len)) || (NULL == nonce) ||
     (NULL == m) || (NULL == a) || (NULL == result))
  {
    return;
  }

  /***************************************************************************/
  /* AES peripheral initialization */
  /***************************************************************************/
  /* Initialize the driver. */
  AESCCM_init();
  AESCCM_Params_init(&params);
  params.returnBehavior = AESCCM_RETURN_BEHAVIOR_POLLING;
  params.timeout = 0;
  /* Open an instance of the driver. */
  handle = AESCCM_open(AESCCM_INSTANCE, &params);

  /***************************************************************************/
  /* Data encryption/decryption. */
  /***************************************************************************/
  if(handle != NULL)
  {
    /* Initialize key handler. */
    CryptoKeyPlaintext_initKey(&cryptoKey, encKey, AES_128_KEY_LENGTH);
    /* Set up AESCCM_Operation. */
    AESCCM_Operation_init(&operation);
    operation.key               = &cryptoKey;
    operation.aad               = (uint8_t*)a;
    operation.aadLength         = a_len;
    operation.input             = m;
    operation.output            = m;
    operation.inputLength       = m_len;
    operation.nonce             = (uint8_t*)nonce;
    operation.nonceLength       = CCM_STAR_NONCE_LENGTH;
    operation.mac               = result;
    operation.macLength         = mic_len;

    if(1 == forward)
    {
      /* Do a CCM encryption operation in a single call. */
      AESCCM_oneStepEncrypt(handle, &operation);
    }
    else
    {
      /* Do a CCM decryption operation in a single call. */
      encryptionResult = AESCCM_oneStepDecrypt(handle, &operation);

      if(encryptionResult != AESCCM_STATUS_SUCCESS)
      {
        memcpy(result, MIC_INVALID, mic_len);
      }
    }
  }

  /***************************************************************************/
  /* AES peripheral deinitialization. */
  /***************************************************************************/
  /* Close the driver. */
  AESCCM_close(handle);
}

/*---------------------------------------------------------------------------*/
const struct ccm_star_driver hw_ccm_star_driver = {
  set_key,
  aead
};
/*---------------------------------------------------------------------------*/


#ifdef __cplusplus
}
#endif
