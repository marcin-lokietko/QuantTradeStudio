import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { AccountRoutingModule } from './account-routing.module';
import { AssetsPage } from './pages/assets/assets-page.component';
import { OrdersPage } from './pages/orders/orders-page.component';
import { MatIconModule } from '@angular/material/icon';
import { MatTableModule } from '@angular/material/table';
import { MatSortModule } from '@angular/material/sort';
import { MatButtonModule } from '@angular/material/button';
import { MatMenuModule } from '@angular/material/menu';
import { MatDialogModule } from '@angular/material/dialog';
import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';

@NgModule({
  declarations: [AssetsPage, OrdersPage],
  imports: [CommonModule, AccountRoutingModule, MatTableModule, MatSortModule, MatIconModule, MatButtonModule, MatMenuModule, MatDialogModule, MatProgressSpinnerModule],
  exports: [],
})
export class AccountModule {}