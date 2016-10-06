/*
	This file is part of the UrgBenri application.

	Copyright (c) 2016 Mehrez Kristou.
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Please contact kristou@hokuyo-aut.jp for more details.

*/

#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

/*!
  \file
  \brief 移動平均の計算

  \author Satofumi KAMIMURA

  $Id: MovingAverage.h 57 2012-06-12 04:43:55Z kristou $
*/

#include <deque>


/*!
  \brief 移動平均のテンプレート

  \attention 和が使用している型より大きな値になると、結果が保証されない
*/
template <class T>
class MovingAverage
{
public:
    /*!
      \brief コンストラクタ

      \param[in] size バッファのサイズ
    */
    MovingAverage(size_t size, T initial_value = 0)
        : m_buffer_size(size), m_total(0) {
        if (m_buffer_size == 0) {
            m_buffer_size = 1;
        }
        buffer_set(m_buffer_size, initial_value);
    }


    /*!
      \brief 格納されている平均値の更新

      \param value [i] 更新する平均値
    */
    void setAverageValue(T value) {
        buffer_set(m_buffer_size, value);
    }


    /*!
      \brief 平均値の取得

      \return 移動平均の値
    */
    T average(void) {
        return m_average;
    }


    /*!
      \brief 値の追加

      \param[in] value 追加する値

      \return 移動平均の値
    */
    T push_back(T value) {
        m_total -= m_buffer.front();
        m_buffer.pop_front();
        m_total += value;
        m_buffer.push_back(value);

        m_average = m_total / m_buffer_size;

        return m_average;
    }

private:
    MovingAverage(const MovingAverage &rhs);
    MovingAverage &operator = (const MovingAverage &rhs);

    std::deque<T> m_buffer;
    long m_buffer_size;
    T m_average;
    T m_total;


    void buffer_set(size_t size, T value) {
        m_buffer.assign(size, value);
        m_average = value;
        m_total = m_average * size;
    }
};

#endif /* !MOVING_AVERAGE_H */

