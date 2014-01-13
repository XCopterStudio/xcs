#include "video_decode.hpp"

videoDecoder::videoDecoder(const AVCodecID avCodec){
	codec = avcodec_find_encoder(avCodec);
	if(!codec){ // TODO: throw exception
		fprintf(stderr, "Could not find encoder\n");
    exit(1);
	}

	context = avcodec_alloc_context3(codec);
	if(!context){ // TODO: throw exception
		fprintf(stderr, "Could not open context\n");
    exit(1);
	}

	if(codec->capabilities&CODEC_CAP_TRUNCATED)
        context->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */
  /* For some codecs, such as msmpeg4 and mpeg4, width and height
     MUST be initialized there because this information is not
     available in the bitstream. */

  /* open it */
	if (avcodec_open2(context, codec, NULL) < 0) { // TODO: throw exception
			fprintf(stderr, "Could not open codec\n");
      exit(1);
  }
}