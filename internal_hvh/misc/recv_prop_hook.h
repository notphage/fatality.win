#pragma once

class recv_prop_hook
{
public:
	recv_prop_hook( RecvProp* prop )
	{
		ZeroMemory( this, sizeof( recv_prop_hook ) );
		m_property = prop;
	}
	~recv_prop_hook()
	{
		m_property->m_ProxyFn = m_original_proxy_fn;
	}

	RecvVarProxyFn hook_prop( const RecvVarProxyFn proxy_fn )
	{
		m_original_proxy_fn = m_property->m_ProxyFn;
		set_proxy_func( proxy_fn );
		return m_original_proxy_fn;
	}

	RecvVarProxyFn get_orig() const
	{
		return m_original_proxy_fn;
	}

	void set_proxy_func( const RecvVarProxyFn proxy_fn ) const
	{
		m_property->m_ProxyFn = proxy_fn;
	}

private:
	RecvProp * m_property = nullptr;
	RecvVarProxyFn m_original_proxy_fn = nullptr;
};
