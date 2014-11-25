#pragma once

template<typename Pred>
bool execute_safe(Pred f)
{
	try{
		f();
	}
	catch (...) {
		return false;
	}

	return true;
}