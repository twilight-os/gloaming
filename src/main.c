#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <ultra.h>
#include <machine.h>
#include <flanterm.h>
#include <flanterm_backends/fb.h>

void kmain(struct ultra_boot_context *bootctx, uint32_t magic)
{
  if(magic != ULTRA_MAGIC) { hcf(); }

  struct ultra_attribute_header *hdrinfo = bootctx->attributes;
  struct ultra_kernel_info_attribute *krnlinfo = NULL;
  struct ultra_framebuffer_attribute *fbinfo = NULL;

  int32_t i = bootctx->attribute_count;

  while(--i >= 0)
    {
      if(hdrinfo->type == ULTRA_ATTRIBUTE_KERNEL_INFO)
        krnlinfo = (struct ultra_kernel_info_attribute *)hdrinfo;
      if(hdrinfo->type == ULTRA_ATTRIBUTE_FRAMEBUFFER_INFO)
        fbinfo = (struct ultra_framebuffer_attribute *)hdrinfo;

      hdrinfo = ULTRA_NEXT_ATTRIBUTE(hdrinfo);
    }

  if(krnlinfo == NULL || fbinfo == NULL) { hcf(); }

  struct ultra_framebuffer *fb = &fbinfo->fb;

  void *framebuffer_ptr = (void *)(uintptr_t)fb->physical_address;
  uint32_t width = fb->width;
  uint32_t height = fb->height;
  uint32_t pitch = fb->pitch;

  uint8_t red_mask_size = 0, red_mask_shift = 0;
  uint8_t green_mask_size = 0, green_mask_shift = 0;
  uint8_t blue_mask_size = 0, blue_mask_shift = 0;

  switch(fb->format)
    {
    case ULTRA_FB_FORMAT_RGB888:
      red_mask_size = 8;
      red_mask_shift = 16;
      green_mask_size = 8;
      green_mask_shift = 8;
      blue_mask_size = 8;
      blue_mask_shift = 0;
      break;
    case ULTRA_FB_FORMAT_BGR888:
      red_mask_size = 8;
      red_mask_shift = 0;
      green_mask_size = 8;
      green_mask_shift = 8;
      blue_mask_size = 8;
      blue_mask_shift = 16;
      break;
    case ULTRA_FB_FORMAT_RGBX8888:
    case ULTRA_FB_FORMAT_XRGB8888:
      red_mask_size = 8;
      red_mask_shift = 16;
      green_mask_size = 8;
      green_mask_shift = 8;
      blue_mask_size = 8;
      blue_mask_shift = 0;
      break;
    default: hcf();
    }

  struct flanterm_context *ft_ctx = flanterm_fb_init(
    NULL, NULL, framebuffer_ptr, width, height, pitch, red_mask_size,
    red_mask_shift, green_mask_size, green_mask_shift, blue_mask_size,
    blue_mask_shift, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 1,
    0, 0, 0);

  const char msg[] = "Hello world\n";

  flanterm_write(ft_ctx, msg, sizeof(msg));

  hcf();
}
