#include "Client.h"

std::shared_ptr<websocket_client> g_pClient = nullptr;
websocketpp::connection_hdl g_cConnection;
uint32_t g_dwCallbackAdresses[callback_numElements] = { 0x0 };


EXPORT bool websocket_connect(const char *szServer)
{
	if (g_pClient || szServer == NULL)
		return false;

	g_pClient = std::shared_ptr<websocket_client>(new websocket_client());

	g_pClient->clear_access_channels(websocketpp::log::alevel::none);
	g_pClient->clear_error_channels(websocketpp::log::alevel::none);

	g_pClient->init_asio();

	{
		g_pClient->set_open_handler([&](websocketpp::connection_hdl hdl)
		{
			uint32_t address = 0x0;
			if ((address = g_dwCallbackAdresses[callback_on_connect]))
				__asm call address
		});

		g_pClient->set_fail_handler([&](websocketpp::connection_hdl hdl)
		{
			websocket_disconnect();

			uint32_t address = 0x0;
			if ((address = g_dwCallbackAdresses[callback_on_fail]))
				__asm call address

		});

		g_pClient->set_close_handler([&](websocketpp::connection_hdl hdl)
		{
			websocket_disconnect();

			uint32_t address = 0x0;
			if ((address = g_dwCallbackAdresses[callback_on_disconnect]))
				__asm call address

		});

		g_pClient->set_message_handler([&](websocketpp::connection_hdl hdl, websocket_message msg)
		{
			uint32_t address = 0x0;
			if ((address = g_dwCallbackAdresses[callback_on_message]))
			{
				try
				{
					auto c_str = msg->get_payload().c_str();
					auto len = msg->get_payload().length();

					__asm push len
					__asm push c_str
					__asm call address
				}
				catch (...)
				{
				}
			}
		});
	}
	

	websocketpp::lib::error_code ec;
	websocket_client::connection_ptr ptr = g_pClient->get_connection(std::string(szServer), ec);
	if (ec)
	{
		g_pClient = nullptr;
		return false;
	}

	g_cConnection = ptr->get_handle();

	g_pClient->connect(ptr);

	websocketpp::lib::thread(&websocket_client::run, g_pClient).detach();
	return true;
}

EXPORT bool websocket_disconnect()
{
	if (!g_pClient)
		return false;

	try
	{
		while (!g_pClient->stopped())
			g_pClient->stop();
	}
	catch (...)
	{
	}
	
	g_pClient = nullptr;

	return true;
}

EXPORT bool websocket_send(const char *szMessage, const size_t dwLen, bool isBinary)
{
	if (!g_pClient || szMessage == 0 || dwLen == 0)
		return false;

	try
	{
		auto p = g_pClient->get_con_from_hdl(g_cConnection);
		if (p->send(std::string(szMessage, dwLen), isBinary ? websocketpp::frame::opcode::binary : websocketpp::frame::opcode::text))
			return false;

		return true;
	}
	catch (...)
	{
	}

	return false;
}

EXPORT bool websocket_isconnected()
{
	if (!g_pClient)
		return false;

	return true;
}

EXPORT bool websocket_registerCallback(uint32_t dwType, uint32_t dwAddress)
{
	if (dwType < 0 || dwType >= callback_numElements)
		return false;

	g_dwCallbackAdresses[dwType] = dwAddress;
	return true;
}
