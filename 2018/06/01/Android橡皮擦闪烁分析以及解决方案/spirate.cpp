void Framebuffer::drawSprite(SkBitmap &bitmap, SkRect dstRect, SkRegion clearRegion) {

    if (ioctl(frameBufferFd, FBIOGET_VSCREENINFO, &varInfo) < 0) {
        LOGD("failed to get %s info", FB_GRAPHIC_FILE);
    }

    //纠正显示缓冲区...

    /////////////////////////////////////////////////////////////////

    uint32_t *buffer = fbSkBuffer[backFramebufferPan];

    SkRegion region;
    clearRegion.op(dstRect.round(), SkRegion::Op::kUnion_Op);
    region.set(clearRegion);

    //去掉不可清除部分...

    //清除需要清空区域
    for (SkRegion::Iterator it(region); !it.done(); it.next()) {
        const auto &r = it.rect();
        drawFramebufferBlit(buffer, r);
    }

    //小工具和橡皮擦相交部分
    SkRegion eraserRegion;
    eraserRegion.setRect(dstRect.round());

    //去掉不能显示橡皮擦的部分...

    //画橡皮擦部
    for (SkRegion::Iterator it(eraserRegion); !it.done(); it.next()) {
        const auto &r = it.rect();
        SkRect rect = SkRect::Make(r);
        SkIRect srcRect = changeRelativeSkRect(rect, dstRect);
        drawSkBitmap(canvas, bitmap, srcRect, rect);
    }

    //切换显示缓冲区
    varInfo.yoffset = varInfo.yres * backFramebufferPan;
    varInfo.activate = FB_ACTIVATE_VBL;
    if (ioctl(frameBufferFd, FBIOPUT_VSCREENINFO, &varInfo) == 0) {
        swapFramebufferPan();
    }

    //DO SOMETHING.

    /////////////////////////////////////////////////////////////////

    //清掉前一个framebuffer的橡皮擦图标
    buffer = fbSkBuffer[backFramebufferPan];
    for (SkRegion::Iterator it(region); !it.done(); it.next()) {
        const auto &r = it.rect();
        drawFramebufferBlit(buffer, r);
    }
}
