/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#pragma once

void* aligned_malloc(size_t sz, size_t alignment);
void aligned_free(void* p);

extern const std::vector<size_t> vecAlignments;
