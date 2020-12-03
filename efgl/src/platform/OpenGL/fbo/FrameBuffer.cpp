#include "efpch.h"

#include "FrameBuffer.h"

namespace efgl {
	FrameBuffer::FrameBuffer() {
		GLCall(glGenFramebuffers(1, &m_RendererID));
	}

	void FrameBuffer::AddColorAttachment(ITargetable& tex, unsigned int slot) {
		Bind();
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, 
			GL_COLOR_ATTACHMENT0 + slot, 
			GL_TEXTURE_2D, 
			tex.GetID(), 
			0));
		Unbind();
	}

	void FrameBuffer::AddDepthStencilBuffer(ITargetable& tex) {
		Bind();
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, tex.GetID(), 0));
		Unbind();
	}

	void FrameBuffer::AddDepthBuffer(ITargetable& tex) {
		Bind();
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex.GetID(), 0));
		Unbind();
	}

	void FrameBuffer::AddStencilBuffer(ITargetable& tex) {
		Bind();
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, tex.GetID(), 0));
		Unbind();
	}

	void FrameBuffer::ClearColor(Color c, float alpha) {
		Bind();
		GLCall(glClearColor(c.r, c.g, c.b, alpha));
		Unbind();
	}

	void FrameBuffer::ClearBuffers(GLbitfield mask) {
		Bind();
		GLCall(glClear(mask));
		Unbind();
	}

	void FrameBuffer::EnableFeatures(GLenum cap) {
		Bind();
		GLCall(glEnable(cap));
		Unbind();
	}

	FrameBuffer::~FrameBuffer() {
		GLCall(glDeleteFramebuffers(1, &m_RendererID));
	}
}