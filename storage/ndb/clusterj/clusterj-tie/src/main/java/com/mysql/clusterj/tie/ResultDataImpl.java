/*
 *  Copyright (C) 2009 Sun Microsystems, Inc.
 *  All rights reserved. Use is subject to license terms.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

package com.mysql.clusterj.tie;

import java.math.BigDecimal;
import java.math.BigInteger;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import java.util.List;

import com.mysql.clusterj.ClusterJFatalInternalException;

import com.mysql.clusterj.core.store.Blob;
import com.mysql.clusterj.core.store.Column;
import com.mysql.clusterj.core.store.ResultData;

import com.mysql.clusterj.core.util.I18NHelper;
import com.mysql.clusterj.core.util.Logger;
import com.mysql.clusterj.core.util.LoggerFactoryService;

import com.mysql.ndbjtie.ndbapi.NdbBlob;
import com.mysql.ndbjtie.ndbapi.NdbOperation;
import com.mysql.ndbjtie.ndbapi.NdbRecAttr;

/**
 *
 */
class ResultDataImpl implements ResultData {

    /** My message translator */
    static final I18NHelper local = I18NHelper
            .getInstance(ResultDataImpl.class);

    /** My logger */
    static final Logger logger = LoggerFactoryService.getFactory()
            .getInstance(ResultDataImpl.class);

    /** Flags for iterating a scan */
    protected final int RESULT_READY = 0;
    protected final int SCAN_FINISHED = 1;
    protected final int CACHE_EMPTY = 2;

    /** The NdbOperation that defines the result */
    private NdbOperation ndbOperation = null;

    /** The NdbRecAttrs that specify the columns to retrieve */
    private NdbRecAttr[] ndbRecAttrs = null;

    /** The flag indicating that there are no more results */
    private boolean nextDone;

    /** The ByteBuffer containing the results */
    private ByteBuffer byteBuffer = null;

    /** Offsets into the ByteBuffer containing the results */
    private int[] offsets = null;

    /** Lengths of the fields in the ByteBuffer containing the results */
    private int[] lengths = null;

    /** Construct the ResultDataImpl based on an NdbOperation and a list of columns
     * to include in the result. The ResultDataImpl is constructed before the operation
     * is defined.
     * @param ndbOperation the NdbOperation
     * @param storeColumns the columns in the result
     */
    public ResultDataImpl(NdbOperation ndbOperation, List<Column> storeColumns) {
        this.ndbOperation = ndbOperation;
        initializeColumns(ndbOperation, storeColumns);
    }

    /** Initialize the operation using the columns that are requested.
     * @param ndbOperation the operation
     * @param storeColumns the columns to be retrieved
     */
    private void initializeColumns(NdbOperation ndbOperation,
            List<Column> storeColumns) {
        // collect the column ids so we can use an indexed array for offsets and lengths
        int capacity = 0;
        int columnId = 0;
        int maximumColumnId = 0;
        for (Column storeColumn: storeColumns) {
            maximumColumnId = Math.max(maximumColumnId, storeColumn.getColumnId());
        }
        offsets = new int[maximumColumnId + 1];
        lengths = new int[maximumColumnId + 1];
        // iterate the list of store columns and create a buffer big enough for all columns
        for (Column storeColumn: storeColumns) {
            columnId = storeColumn.getColumnId();
            int columnSpace = storeColumn.getColumnSpace();
            lengths[columnId] = columnSpace;
            offsets[columnId] = capacity;
            capacity += columnSpace;
        }
        byteBuffer = ByteBuffer.allocateDirect(capacity);
        byteBuffer.order(ByteOrder.nativeOrder());
        // iterate the list of store columns and allocate an NdbRecAttr (via getValue) for each
        ndbRecAttrs = new NdbRecAttr[maximumColumnId + 1];
        for (Column storeColumn: storeColumns) {
            NdbRecAttr ndbRecAttr = null;
            columnId = storeColumn.getColumnId();
            byteBuffer.position(offsets[columnId]);
            if (lengths[columnId] == 0) {
                ndbRecAttr = ndbOperation.getValue(columnId, null);
            } else {
                ndbRecAttr = ndbOperation.getValue(columnId, byteBuffer);
            }
            handleError(ndbRecAttr, ndbOperation);
            ndbRecAttrs[columnId] = ndbRecAttr;
        }
    }

    public boolean next() {
        // NdbOperation has exactly zero or one result. ScanResultDataImpl handles scans...
        if (nextDone) {
            return false;
        } else {
                nextDone = true;
                return true;
        }
    }

    public Blob getBlob(Column storeColumn) {
        NdbBlob ndbBlob = ndbOperation.getBlobHandle(storeColumn.getColumnId());
        handleError(ndbBlob, ndbOperation);
        return new BlobImpl(ndbBlob);
    }

    public boolean getBoolean(Column storeColumn) {
        int index = storeColumn.getColumnId();
        NdbRecAttr ndbRecAttr = ndbRecAttrs[index];
        return Utility.getBoolean(storeColumn, ndbRecAttr);
    }

    public boolean[] getBooleans(Column storeColumn) {
        throw new ClusterJFatalInternalException(local.message("ERR_Not_Implemented"));
    }

    public byte getByte(Column storeColumn) {
        int index = storeColumn.getColumnId();
        NdbRecAttr ndbRecAttr = ndbRecAttrs[index];
        return Utility.getByte(storeColumn, ndbRecAttr);
    }

    public short getShort(Column storeColumn) {
        int index = storeColumn.getColumnId();
        NdbRecAttr ndbRecAttr = ndbRecAttrs[index];
        return Utility.getShort(storeColumn, ndbRecAttr);
     }

    public int getInt(Column storeColumn) {
        int index = storeColumn.getColumnId();
        NdbRecAttr ndbRecAttr = ndbRecAttrs[index];
        return Utility.getInt(storeColumn, ndbRecAttr);
    }

    public long getLong(Column storeColumn) {
        int index = storeColumn.getColumnId();
        NdbRecAttr ndbRecAttr = ndbRecAttrs[index];
        return Utility.getLong(storeColumn, ndbRecAttr);
     }

    public float getFloat(Column storeColumn) {
        int index = storeColumn.getColumnId();
        float result = ndbRecAttrs[index].float_value();
        return result;
    }

    public double getDouble(Column storeColumn) {
        int index = storeColumn.getColumnId();
        double result = ndbRecAttrs[index].double_value();
        return result;
    }

    public String getString(Column storeColumn) {
        byte[] bytes = getBytes(storeColumn);
        return storeColumn.decode(bytes);
    }

    public byte[] getBytes(Column storeColumn) {
        int index = storeColumn.getColumnId();
        NdbRecAttr ndbRecAttr = ndbRecAttrs[index];
        if (ndbRecAttr.isNULL() == 1) return null;
        int prefixLength = storeColumn.getPrefixLength();
        int actualLength = lengths[index];
        int offset = offsets[index];
        switch (prefixLength) {
            case 0:
                break;
            case 1:
                actualLength = (byteBuffer.get(offset) + 256) % 256;
                offset += 1;
                break;
            case 2:
                actualLength = (byteBuffer.get(offset) + 256) % 256;
                int length2 = (byteBuffer.get(offset + 1) + 256) % 256;
                actualLength += 256 * length2;
                offset += 2;
                break;
            default:
                throw new ClusterJFatalInternalException(
                        local.message("ERR_Invalid_Prefix_Length", prefixLength));
        }
        byteBuffer.position(offset);
        byte[] result = new byte[actualLength];
        byteBuffer.get(result);
        return result;
     }


    public Object getObject(Column storeColumn) {
        throw new ClusterJFatalInternalException(local.message("ERR_Implementation_Should_Not_Occur"));
    }

    public boolean wasNull(Column storeColumn) {
        throw new ClusterJFatalInternalException(local.message("ERR_Implementation_Should_Not_Occur"));
    }

    public Boolean getObjectBoolean(Column storeColumn) {
        int index = storeColumn.getColumnId();
        NdbRecAttr ndbRecAttr = ndbRecAttrs[index];
        if (ndbRecAttr.isNULL() == 1) {
            return null;
        } else {
            byte value = ndbRecAttr.int8_value();
            Boolean result = (Boolean.valueOf((value & 0x01) == 0x01));
            return result;
        }
    }

    public Byte getObjectByte(Column storeColumn) {
        int index = storeColumn.getColumnId();
        NdbRecAttr ndbRecAttr = ndbRecAttrs[index];
        return (ndbRecAttr.isNULL() == 1)?null:Utility.getByte(storeColumn, ndbRecAttr);
    }

    public Short getObjectShort(Column storeColumn) {
        int index = storeColumn.getColumnId();
        NdbRecAttr ndbRecAttr = ndbRecAttrs[index];
        return (ndbRecAttr.isNULL() == 1)?null:Utility.getShort(storeColumn, ndbRecAttr);
    }

    public Integer getObjectInteger(Column storeColumn) {
        int index = storeColumn.getColumnId();
        NdbRecAttr ndbRecAttr = ndbRecAttrs[index];
        return (ndbRecAttr.isNULL() == 1)?null:Utility.getInt(storeColumn, ndbRecAttr);
    }

    public Long getObjectLong(Column storeColumn) {
        int index = storeColumn.getColumnId();
        NdbRecAttr ndbRecAttr = ndbRecAttrs[index];
        return (ndbRecAttr.isNULL() == 1)?null:Utility.getLong(storeColumn, ndbRecAttr);
    }

    public Float getObjectFloat(Column storeColumn) {
        int index = storeColumn.getColumnId();
        NdbRecAttr ndbRecAttr = ndbRecAttrs[index];
        return (ndbRecAttr.isNULL() == 1)?null:getFloat(storeColumn);
    }

    public Double getObjectDouble(Column storeColumn) {
        int index = storeColumn.getColumnId();
        NdbRecAttr ndbRecAttr = ndbRecAttrs[index];
        return (ndbRecAttr.isNULL() == 1)?null:getDouble(storeColumn);
    }

    public BigInteger getBigInteger(Column storeColumn) {
        int index = storeColumn.getColumnId();
        int offset = offsets[index];
        int precision = storeColumn.getPrecision();
        int scale = storeColumn.getScale();
        int length = Utility.getDecimalColumnSpace(precision, scale);
        byteBuffer.position(offset);
        return Utility.getBigInteger(byteBuffer, length, precision, scale);
    }

    public BigDecimal getDecimal(Column storeColumn) {
        int index = storeColumn.getColumnId();
        int offset = offsets[index];
        int precision = storeColumn.getPrecision();
        int scale = storeColumn.getScale();
        int length = Utility.getDecimalColumnSpace(precision, scale);
        byteBuffer.position(offset);
        return Utility.getDecimal(byteBuffer, length, precision, scale);
    }

    private void handleError(Object object, NdbOperation ndbOperation) {
        if (object == null) {
            Utility.throwError(object, ndbOperation.getNdbError());
        }
    }

    public void setNoResult() {
        nextDone = true;
    }

}
