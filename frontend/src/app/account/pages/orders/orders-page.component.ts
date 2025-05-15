import { MatTableDataSource } from '@angular/material/table';
import { MatSort } from '@angular/material/sort';
import { ChangeDetectorRef, Component, ViewChild } from '@angular/core';
import { environment } from '@env/environment';

export interface Order {
  orderId: number;
  assetPair: string;
  origQuantity: string;
  executedQuantity: string;
  orderSide: string;
  price: string;
}

@Component({
  selector: 'app-orders-page',
  standalone: false,
  templateUrl: './orders-page.component.html',
  styleUrls: ['./orders-page.component.scss'],
})
export class OrdersPage {
  public orders: Order[] = [];
  public isLoading = true;
  public displayedColumns: string[] = ['assetPair', 'orderSide', 'price', 'origQuantity', 'executedQuantity'];

  dataSource = new MatTableDataSource(this.orders);
  @ViewChild(MatSort) sort!: MatSort;

  constructor(private cdr: ChangeDetectorRef) {}

  ngOnInit(): void {
    this.getData();
  }

  getData(): void {
    fetch(environment.algoTraderBackendUrlPrefix + '/openOrders')
      .then((response) => {
        this.isLoading = false;
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.json();
      })
      .then((data) => {
        try {
          this.orders = data as Order[];
          this.dataSource = new MatTableDataSource(this.orders);
          this.dataSource.sort = this.sort;
          this.cdr.markForCheck();
        } catch (error) {
          console.error('Invalid orders content:', error);
        }
      })
      .catch((error) => {
        console.error('Error fetching data:', error);
      });
  }
}
