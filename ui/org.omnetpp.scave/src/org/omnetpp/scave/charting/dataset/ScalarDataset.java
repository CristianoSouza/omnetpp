package org.omnetpp.scave.charting.dataset;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarResult;

/**
 * Class storing the dataset of a scalar chart.
 *
 * @author tomi
 */
public class ScalarDataset implements IScalarDataset {

    /** The row keys. */
    private List<Comparable> rowKeys;

    /** The column keys. */
    private List<Comparable> columnKeys;

    /** The row data. */
    private List<List<Double>> rows;

    /**
     * Creates an empty dataset.
     */
    public ScalarDataset() {
        this.rowKeys = new ArrayList<Comparable>();
        this.columnKeys = new ArrayList<Comparable>();
        this.rows = new ArrayList<List<Double>>();
    }
    
    /**
     * Creates a dataset from the given scalars.
     * The 
     * @param idlist   the scalars contained by this dataset
     * @param manager
     */
    public ScalarDataset(IDList idlist, ResultFileManager manager) {
    	this();
    	addScalars(idlist, manager);
    }

    /**
     * Returns the row count.
     *
     * @return The row count.
     */
    public int getRowCount() {
        return this.rowKeys.size();
    }
    
    
    /**
     * Returns the key for a given row.
     *
     * @param row  the row index (zero based).
     *
     * @return The row index.
     */
    public Comparable getRowKey(int row) {
        return this.rowKeys.get(row);
    }

    /**
     * Returns the column count.
     *
     * @return The column count.
     */
    public int getColumnCount() {
        return this.columnKeys.size();
    }
    
    /**
     * Returns the key for a given column.
     *
     * @param column  the column.
     *
     * @return The key.
     */
    public Comparable getColumnKey(int column) {
        return this.columnKeys.get(column);
    }

    /**
     * Returns the value for a given row and column.
     *
     * @param row  the row index.
     * @param column  the column index.
     *
     * @return The value.
     */
    public double getValue(int row, int column) {
        double result = Double.NaN;
        if (row >= 0 && row < rows.size()) {
            List<Double> rowData = this.rows.get(row);
            if (column >= 0 && column < rowData.size())
        	result = rowData.get(column);
        }
        return result;
    }
    
    private void addScalars(IDList idlist, ResultFileManager manager) {
		for (int i = 0; i < idlist.size(); ++i) {
			ScalarResult scalar = manager.getScalar(idlist.get(i));
			addValue(scalar.getFileRun().getRun().getRunName(),
			         scalar.getModuleName()+"\n"+scalar.getName(),
				     scalar.getValue());
		}
    }
    
    private void addValue(Comparable rowKey, Comparable columnKey, double value) {
    	int rowIndex = this.rowKeys.indexOf(rowKey);
    	List<Double> rowData;
    	if (rowIndex >= 0)
    		rowData = this.rows.get(rowIndex);
    	else
    		rowData = addRow(rowKey);
    	int columnIndex = this.columnKeys.indexOf(columnKey);
    	if (columnIndex < 0)
    		columnIndex = addColumn(columnKey);
    	rowData.set(columnIndex, value);
    }
    
    private List<Double> addRow(Comparable rowKey) {
    	List<Double> rowData = new ArrayList<Double>();
    	int c = getColumnCount();
    	for (int i = 0; i < c; ++i)
    		rowData.add(Double.NaN);
    	this.rowKeys.add(rowKey);
    	this.rows.add(rowData);
    	return rowData;
    }
    
    private int addColumn(Comparable columnKey) {
    	int c = getColumnCount();
    	this.columnKeys.add(columnKey);
    	for (List<Double> rowData : this.rows)
    		rowData.add(Double.NaN);
    	return c;
    }
}
