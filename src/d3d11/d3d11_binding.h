#pragma once

#include "d3d11_context_state.h"

namespace dxvk {

  template<typename T, uint32_t Count>
  class D3D11BindInfo {

  public:

    template<typename Tx>
    D3D11BindInfo(const Tx* pArgs, uint32_t slot)
    : m_slot(slot),
      m_head(pArgs[slot]),
      m_tail(pArgs, slot + 1u) { }

    template<typename Tx>
    D3D11BindInfo(const Tx* pArgs, const uint32_t* pSlots)
    : m_slot(pSlots[0]),
      m_head(pArgs[m_slot]),
      m_tail(pArgs, pSlots + 1) { }

    void bind(DxvkContext* ctx, uint32_t firstSlot) const {
      m_head.bind(ctx, firstSlot + m_slot);
      m_tail.bind(ctx, firstSlot);
    }

  private:

    uint32_t                    m_slot;
    T                           m_head;
    D3D11BindInfo<T, Count - 1> m_tail;

  };

  template<typename T>
  class D3D11BindInfo<T, 0> {

  public:

    template<typename Tx>
    D3D11BindInfo(const Tx*, uint32_t) {

    }

    template<typename Tx>
    D3D11BindInfo(const Tx*, const uint32_t*) {

    }

    void bind(DxvkContext* ctx, uint32_t firstSlot) const {

    }

  };

  class D3D11SamplerBindInfo {

  public:

    D3D11SamplerBindInfo(const Com<D3D11SamplerState>& binding)
    : m_sampler(binding != nullptr ? binding->GetDXVKSampler() : nullptr) {

    }

    void bind(DxvkContext* ctx, uint32_t slotId) const {
      ctx->bindResourceSampler(slotId, m_sampler);
    }

  private:

    Rc<DxvkSampler> m_sampler;

  };

  class D3D11ConstantBufferBindInfo {

  public:

    D3D11ConstantBufferBindInfo(const D3D11ConstantBufferBinding& binding)
    : m_slice(binding.constantBound
        ? binding.buffer->GetBufferSlice(16 * binding.constantOffset, 16 * binding.constantBound)
        : DxvkBufferSlice()) {

    }

    void bind(DxvkContext* ctx, uint32_t slotId) const {
      ctx->bindResourceBuffer(slotId, m_slice);
    }

  private:

    DxvkBufferSlice m_slice;

  };

  class D3D11ShaderResourceBindInfo {

  public:

    D3D11ShaderResourceBindInfo(const Com<D3D11ShaderResourceView>& binding)
    : m_imageView(binding != nullptr ? binding->GetImageView() : nullptr),
      m_bufferView(binding != nullptr ? binding->GetBufferView() : nullptr) {

    }

    void bind(DxvkContext* ctx, uint32_t slotId) const {
      ctx->bindResourceView(slotId, m_imageView, m_bufferView);
    }

  private:

    Rc<DxvkImageView>  m_imageView;
    Rc<DxvkBufferView> m_bufferView;

  };

  class D3D11VertexBufferBindInfo {

  public:

    D3D11VertexBufferBindInfo(const D3D11VertexBufferBinding& binding)
    : m_slice(binding.buffer != nullptr ? binding.buffer->GetBufferSlice(binding.offset) : DxvkBufferSlice()),
      m_stride(binding.stride) {

    }

    void bind(DxvkContext* ctx, uint32_t slotId) const {
      ctx->bindVertexBuffer(slotId, m_slice, m_stride);
    }

  private:

    DxvkBufferSlice m_slice;
    uint32_t        m_stride;

  };

}
