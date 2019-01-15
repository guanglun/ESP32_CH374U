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

#define KEY_LEN 128*2

//#define LOCAL_MBEDTLS_MPI_CHK(f) do { ret = f ; printf("ret %d\r\n",ret); } while( 0 )
#define LOCAL_MBEDTLS_MPI_CHK(f) do { ret = f ;} while( 0 )

#define  RSA_N 	"00e3028bca8e63c1cf8343b16d4e443fbaef4af08106526e8a42cc957081685b6ba685e127382407d9b817382e4d0b173885568aca009fc945be483cf7d4fdd377a12ce42a66c6642dbf4de3ea7494f135ff21d31792a94a0c3158008e1663594ea116e26b36bd3fc376b319267fd30e330fdc00f25f7ff3eda929d5ff542eaf74cab5dab1d81286db661b41e3f9450fe9e3eff33b124afcebbc1fd75fc519b4171700e899a6071b3a628ced0eabbba20946ef405f3dda7d068d0f38195e261ae5d92790893e5d4fab8197364b42313e6d4b0c8be33943bea6d4e9399b75d681300189f37d61d5aad14e6f4fda9ebab10b977f4ff135186f7b9b013e85d4246e31"
#define  RSA_E 	"010001"
#define  RSA_D 	"01ce206d8d5fd10161c97d118e9986099d5d060b47d123b164454b4f2045ea1efe7a40f7733949b9b18b6aec9a21f031fbb775692e1d30c4ff8f5132cc52d90031fbf75a51f7340208ebf617c43f76c294502765200b16e2cd6f72dd47fd31f5a9426013dae1d47aed155d9d48b5d564ec413677508aff9606e14bd64c18b9f157a03d78e1925a8091cc4548b04f308f847ec97a998d887c23ae481de03ed6bea25e794181b136f599d3db826bb69217f76d83a9852a1161417a90c6ae7c375f1a0719154a44d09a6926b5781297c61efd38f6959157731ae2f6e158ecb6c070c84de04e7024ee7b2f99fd03d1678441365357a51e0c3d443b39087161aac935"
#define  RSA_P 	"00fe2903bd468220a5313bee8609b9dff122c473882a77eec45ecb90b09131f4e6995fd0a3219f05b7f58a7c42d46ab65935dbeba07c2836a197408fb6017e10487803789398825934fc00c2be9ff7089a6a68c0f63de05ca84e4d901909fd7a1b152ae2368f6d7d1b28d9f766e9c4e5118ed694706ff30fb2fadbe016abd23d35"
#define  RSA_Q 	"00e4a7381bc5653bd5242065962f11b91dfc7b8e57fd4c96e612d33bc687a06d24e424adf793028c3ae2c6977750b9c4bd7db7f1b8f8c4a644e75ad995da9b4a7d4103e9ca6515d9719b82d7790d7cbd45881edd67ad2716477cb9ea65ba7ab603703c48a7a42c6269349a52c677dc19f7ee66ad794e9c6a236894f12df164d88d"

uint8_t input_sha1[] = {0x30,0x21,0x30,0x09,0x06,0x05,0x2b,0x0e,0x03,0x02,0x1a,0x05,0x00,0x04,0x14,0x6d,0x78,0x26,0xd7,0x5b,0x2b,0xc3,0x70,0x8a,0x23,0x0e,0xc1,0x36,0xe1,0x40,0xc9,0xa2,0xd3,0x71,0x0d};

#if defined(MBEDTLS_PKCS1_V15)
static int myrand( void *rng_state, unsigned char *output, size_t len )
{
#if !defined(__OpenBSD__)
    size_t i;

    if( rng_state != NULL )
        rng_state  = NULL;

    for( i = 0; i < len; ++i )
        output[i] = rand();
#else
    if( rng_state != NULL )
        rng_state = NULL;

    arc4random_buf( output, len );
#endif /* !OpenBSD */

    return( 0 );
}
#endif /* MBEDTLS_PKCS1_V15 */

void SHA1withRSA(uint8_t *input_buffer,uint16_t input_len,uint8_t *output_buffer)
{
	int ret;
	mbedtls_mpi K;
    mbedtls_rsa_context rsa;

    printf("RSA work start\r\n");

    memcpy(input_sha1 + 15,input_buffer,input_len);

	mbedtls_mpi_init( &K );
    mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );

    LOCAL_MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &K, 16, RSA_N  ) );
    LOCAL_MBEDTLS_MPI_CHK( mbedtls_rsa_import( &rsa, &K, NULL, NULL, NULL, NULL ) );
    LOCAL_MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &K, 16, RSA_P  ) );
    LOCAL_MBEDTLS_MPI_CHK( mbedtls_rsa_import( &rsa, NULL, &K, NULL, NULL, NULL ) );
    LOCAL_MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &K, 16, RSA_Q  ) );
    LOCAL_MBEDTLS_MPI_CHK( mbedtls_rsa_import( &rsa, NULL, NULL, &K, NULL, NULL ) );
    LOCAL_MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &K, 16, RSA_D  ) );
    LOCAL_MBEDTLS_MPI_CHK( mbedtls_rsa_import( &rsa, NULL, NULL, NULL, &K, NULL ) );
    LOCAL_MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &K, 16, RSA_E  ) );
    LOCAL_MBEDTLS_MPI_CHK( mbedtls_rsa_import( &rsa, NULL, NULL, NULL, NULL, &K ) );

    LOCAL_MBEDTLS_MPI_CHK( mbedtls_rsa_complete( &rsa ) );

	//printf( "mbedtls_rsa_check_pubkey start\r\n" );
	LOCAL_MBEDTLS_MPI_CHK( mbedtls_rsa_check_pubkey(  &rsa ) );
	LOCAL_MBEDTLS_MPI_CHK( mbedtls_rsa_check_privkey(  &rsa ) );

	//printf( "mbedtls_rsa_pkcs1_encrypt start\r\n" );
    LOCAL_MBEDTLS_MPI_CHK( mbedtls_rsa_pkcs1_encrypt( &rsa, myrand, NULL, MBEDTLS_RSA_PRIVATE,
                                   sizeof(input_sha1), input_sha1,
                                   output_buffer));
	
	//printf_byte(output_buffer,KEY_LEN);

	printf("RSA work done\r\n");

    mbedtls_mpi_free( &K );
    mbedtls_rsa_free( &rsa );
}

