#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fb.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Seu Nome");
MODULE_DESCRIPTION("Módulo de kernel para capturar prints de tela");

static struct fb_info *fbinfo = NULL;

static void screenshot_capture(void)
{
    struct fb_var_screeninfo var_info = fbinfo->var;
    unsigned int x, y;
    unsigned int bpp = var_info.bits_per_pixel;
    unsigned long size = var_info.xres_virtual * var_info.yres_virtual * bpp / 8;
    char *buffer = kmalloc(size, GFP_KERNEL);

    if (!buffer) {
        pr_err("Falha ao alocar memória para o buffer.\n");
        return;
    }

    for (y = 0; y < var_info.yres_virtual; y++) {
        for (x = 0; x < var_info.xres_virtual; x++) {
            unsigned int offset = (y * var_info.xres_virtual + x) * bpp / 8;
            unsigned int pixel;

            if (copy_from_user(&pixel, (void __user *)(fbinfo->screen_base + offset), bpp / 8) != 0) {
                pr_err("Erro ao ler o framebuffer.\n");
                kfree(buffer);
                return;
            }

            // Faça o que quiser com o pixel capturado aqui
            // Por exemplo, você pode armazená-lo no buffer para uso posterior
        }
    }

    kfree(buffer);
}

static int __init screenshot_init(void)
{
    fbinfo = framebuffer_alloc(0, NULL);
    if (!fbinfo) {
        pr_err("Não foi possível alocar informações do framebuffer.\n");
        return -ENOMEM;
    }

    if (fb_set_var(fbinfo, &fbinfo->var)) {
        pr_err("Falha ao definir as variáveis do framebuffer.\n");
        framebuffer_release(fbinfo);
        return -EINVAL;
    }

    if (fb_set_cmap(&fbinfo->cmap, fbinfo)) {
        pr_err("Falha ao definir a tabela de cores do framebuffer.\n");
        framebuffer_release(fbinfo);
        return -EINVAL;
    }

    screenshot_capture();

    pr_info("Captura de tela realizada com sucesso.\n");

    return 0;
}

static void __exit screenshot_exit(void)
{
    framebuffer_release(fbinfo);

    pr_info("Módulo de captura de tela sendo removido.\n");
}

module_init(screenshot_init);
module_exit(screenshot_exit);
