#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "mbedtls/rsa.h"
#include "mbedtls/rsa_internal.h"
#include "mbedtls/oid.h"
#include "mbedtls/platform_util.h"

#include "log.h"

#define KEY_LEN 128 * 2

//#define LOCAL_MBEDTLS_MPI_CHK(f) do { ret = f ; ESP_LOGI("ATouch", "ret %d",ret); } while( 0 )
#define LOCAL_MBEDTLS_MPI_CHK(f) \
    do                           \
    {                            \
        ret = f;                 \
    } while (0)

#define  RSA_N 	"00e3028bca8e63c1cf8343b16d4e443fbaef4af08106526e8a42cc957081685b6ba685e127382407d9b817382e4d0b173885568aca009fc945be483cf7d4fdd377a12ce42a66c6642dbf4de3ea7494f135ff21d31792a94a0c3158008e1663594ea116e26b36bd3fc376b319267fd30e330fdc00f25f7ff3eda929d5ff542eaf74cab5dab1d81286db661b41e3f9450fe9e3eff33b124afcebbc1fd75fc519b4171700e899a6071b3a628ced0eabbba20946ef405f3dda7d068d0f38195e261ae5d92790893e5d4fab8197364b42313e6d4b0c8be33943bea6d4e9399b75d681300189f37d61d5aad14e6f4fda9ebab10b977f4ff135186f7b9b013e85d4246e31"
#define  RSA_E 	"010001"
#define  RSA_D 	"01ce206d8d5fd10161c97d118e9986099d5d060b47d123b164454b4f2045ea1efe7a40f7733949b9b18b6aec9a21f031fbb775692e1d30c4ff8f5132cc52d90031fbf75a51f7340208ebf617c43f76c294502765200b16e2cd6f72dd47fd31f5a9426013dae1d47aed155d9d48b5d564ec413677508aff9606e14bd64c18b9f157a03d78e1925a8091cc4548b04f308f847ec97a998d887c23ae481de03ed6bea25e794181b136f599d3db826bb69217f76d83a9852a1161417a90c6ae7c375f1a0719154a44d09a6926b5781297c61efd38f6959157731ae2f6e158ecb6c070c84de04e7024ee7b2f99fd03d1678441365357a51e0c3d443b39087161aac935"
#define  RSA_P 	"00fe2903bd468220a5313bee8609b9dff122c473882a77eec45ecb90b09131f4e6995fd0a3219f05b7f58a7c42d46ab65935dbeba07c2836a197408fb6017e10487803789398825934fc00c2be9ff7089a6a68c0f63de05ca84e4d901909fd7a1b152ae2368f6d7d1b28d9f766e9c4e5118ed694706ff30fb2fadbe016abd23d35"
#define  RSA_Q 	"00e4a7381bc5653bd5242065962f11b91dfc7b8e57fd4c96e612d33bc687a06d24e424adf793028c3ae2c6977750b9c4bd7db7f1b8f8c4a644e75ad995da9b4a7d4103e9ca6515d9719b82d7790d7cbd45881edd67ad2716477cb9ea65ba7ab603703c48a7a42c6269349a52c677dc19f7ee66ad794e9c6a236894f12df164d88d"

// #define RSA_N "00ed088458c3ede3e9268cd683c96e785f4634950c08bd7a6e9e63a0112dff53cddc0acc0ad2e086767504f66ac8bf00ec26a2d9f4299fbb6465e55d60a7df0fcdc567be037d6ea717b81916198d6bbdbb2718af2332c4144957ea5253da8332f05ea72e34c75feb2bf7a9cbf89060aea539f8111ea5258dc130cf7eaf6919343d5affcff55229d088b2eaf20333a9424e13cb636c730e84b52189ac9fbdb9c53895e32337d21ef42f40b269ad74bebccafd42178886fe5a372adfd6128786fb0984a85020a48195885ccc681632ddfe5fdfeeb8166f277131d79d00613568e76712b8b87185b0624ae12865de4a2b206f01a042b6d5c3559e124dd571482dbbd9"
// #define RSA_E "10001"
// #define RSA_D "4d1a4622b1a90247e6f84d171540cffafdd540de67416b3fec59afc9a6d2b529f377b7a395b0df4c4d084e37b2111f75b1a3ab8e16b414bab5c2843b5a9fde7e2ac67232a46c8801d92a9fdbb3fae5fea8db08ad44682fc923c5defdd3c8759b66ceaee3102f8d4a0207c387993f39d019292e386fb3e58680201eaf645a8479088b7db4bd26b6179399419fbf490d20958d99fefb62a92bebd96403ceaefdcddb900224c7e290d0dbb1f8a70adab80b9002423e5a7d41945c27ecfde6da215f0c6a2aa96706f8fbd6e5c3a4dd4122ac40dd4dfa0ab0f16c103692c6d78db47a46d36009739e443b0e53bf419d728f186aa8790a7a5d9ecf7bf5f400bc1b06e1"
// #define RSA_P "00f827d3adddeb8ed1f3fe8573b80f70a083e4f68c96e8ae82071fe32e4296d852b2498ed4d206876e8d6e6641560c612053578c76f79432c218007959c663122b87e467d2b69a87bd10254f19ccc0ac93ff2a58d22f0f39f8bbe61fa374622112183af7db89f2d08bdc9733c854cd139b72127c1ae8fffc27ebe6026b87744bed"
// #define RSA_Q "00f486af1be060214fa4b5f02fe218d14002de563e4156398d52e2d5e4d3d8baee912051ebe5ff1a2a0ecf3cbd47b8977d82b8df5ca11ebd35a5b5eb2b8f105b5e06a5520ebe5e2deb634e467792983155181bb0552cad43f6d332d7d67ce207279e44e4a03806f7c795d683b43f6f3d6cf643d48d3bc132a2e8b01de042b56a1d"

 uint8_t input_sha1[15 + 20] = {0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14, 0x6d, 0x78, 0x26, 0xd7, 0x5b, 0x2b, 0xc3, 0x70, 0x8a, 0x23, 0x0e, 0xc1, 0x36, 0xe1, 0x40, 0xc9, 0xa2, 0xd3, 0x71, 0x0d};

#if defined(MBEDTLS_PKCS1_V15)
static int myrand(void *rng_state, unsigned char *output, size_t len)
{
#if !defined(__OpenBSD__)
    size_t i;

    if (rng_state != NULL)
        rng_state = NULL;

    for (i = 0; i < len; ++i)
        output[i] = rand();
#else
    if (rng_state != NULL)
        rng_state = NULL;

    arc4random_buf(output, len);
#endif /* !OpenBSD */

    return (0);
}
#endif /* MBEDTLS_PKCS1_V15 */

int SHA1withRSA(uint8_t *input_buffer, uint16_t input_len, uint8_t *output_buffer)
{
    int ret;
    mbedtls_mpi K;
    mbedtls_rsa_context rsa;

    ESP_LOGI("ATouch", "RSA work start");

    memcpy(input_sha1 + 15, input_buffer, input_len);

    mbedtls_mpi_init(&K);
    mbedtls_rsa_init(&rsa, MBEDTLS_RSA_PKCS_V15, 0);

    LOCAL_MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&K, 16, RSA_N));
    LOCAL_MBEDTLS_MPI_CHK(mbedtls_rsa_import(&rsa, &K, NULL, NULL, NULL, NULL));
    LOCAL_MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&K, 16, RSA_P));
    LOCAL_MBEDTLS_MPI_CHK(mbedtls_rsa_import(&rsa, NULL, &K, NULL, NULL, NULL));
    LOCAL_MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&K, 16, RSA_Q));
    LOCAL_MBEDTLS_MPI_CHK(mbedtls_rsa_import(&rsa, NULL, NULL, &K, NULL, NULL));
    LOCAL_MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&K, 16, RSA_D));
    LOCAL_MBEDTLS_MPI_CHK(mbedtls_rsa_import(&rsa, NULL, NULL, NULL, &K, NULL));
    LOCAL_MBEDTLS_MPI_CHK(mbedtls_mpi_read_string(&K, 16, RSA_E));
    LOCAL_MBEDTLS_MPI_CHK(mbedtls_rsa_import(&rsa, NULL, NULL, NULL, NULL, &K));

    LOCAL_MBEDTLS_MPI_CHK(mbedtls_rsa_complete(&rsa));

    //ESP_LOGI("ATouch",  "mbedtls_rsa_check_pubkey start" );
    LOCAL_MBEDTLS_MPI_CHK(mbedtls_rsa_check_pubkey(&rsa));
    LOCAL_MBEDTLS_MPI_CHK(mbedtls_rsa_check_privkey(&rsa));

    //ESP_LOGI("ATouch",  "mbedtls_rsa_pkcs1_encrypt start" );

    //mbedtls_rsa_rsaes_pkcs1_v15_encrypt
    LOCAL_MBEDTLS_MPI_CHK(mbedtls_rsa_pkcs1_encrypt(&rsa, myrand, NULL, MBEDTLS_RSA_PRIVATE,
                                                    sizeof(input_sha1), input_sha1,
                                                    output_buffer));

    ESP_LOGD("ATouch", "RSA IN: %d",input_len);
    printf_byte(input_buffer,input_len);

    ESP_LOGD("ATouch", "RSA OUT: %d",KEY_LEN);
    printf_byte(output_buffer,KEY_LEN);

    ESP_LOGI("ATouch", "RSA work done");

    mbedtls_mpi_free(&K);
    mbedtls_rsa_free(&rsa);

    return ret;
}
