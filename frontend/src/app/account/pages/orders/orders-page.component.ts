import { MatTableDataSource } from '@angular/material/table';
import { MatSort } from '@angular/material/sort';
import { ChangeDetectorRef, Component, ViewChild } from '@angular/core';
import { DataService, Order } from '@app/services/data.service';

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

  constructor(
    private cdr: ChangeDetectorRef,
    private dataService: DataService,
  ) {}

  ngOnInit(): void {
    this.getData();
  }

  getData(): void {
    this.dataService.getOpenOrders().then((data) => {
      this.isLoading = false;
      if (data) {
        this.orders = data;
        this.dataSource = new MatTableDataSource(this.orders);
        this.dataSource.sort = this.sort;
        this.cdr.markForCheck();
      }
    });
  }
}
